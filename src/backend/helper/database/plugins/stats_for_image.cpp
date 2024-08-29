///
/// \file      stats_for_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "stats_for_image.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toTable(const QueryFilter &filter) -> joda::table::Table
{
  table::Table results;

  std::unique_ptr<duckdb::QueryResult> stats = filter.analyzer.select(
      "SELECT"
      "  SUM(element_at(values, $1)[1]) as val_sum,"
      "  MIN(element_at(values, $1)[1]) as val_min,"
      "  MAX(element_at(values, $1)[1]) as val_max,"
      "  AVG(element_at(values, $1)[1]) as val_avg,"
      "  STDDEV(element_at(values, $1)[1]) as val_stddev "
      "FROM objects "
      "WHERE"
      " objects.image_id=$2 AND objects.validity=0 AND objects.channel_id=$3 ",
      (uint16_t) filter.measurement, filter.imageId, static_cast<uint8_t>(filter.clusterId));

  if(stats->HasError()) {
    throw std::invalid_argument(stats->GetError());
  }

  auto materializedResult = stats->Cast<duckdb::StreamQueryResult>().Materialize();
  results.setColHeader({{0, toString(filter.measurement)}});

  results.getMutableRowHeader()[0] = "sum";
  results.getMutableRowHeader()[1] = "min";
  results.getMutableRowHeader()[2] = "max";
  results.getMutableRowHeader()[3] = "avg";
  results.getMutableRowHeader()[4] = "stddev";
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    try {
      results.setData(0, n, table::TableCell{materializedResult->GetValue(0, 0).GetValue<double>(), 0, true, ""});
      results.setData(1, n, table::TableCell{materializedResult->GetValue(1, 0).GetValue<double>(), 0, true, ""});
      results.setData(2, n, table::TableCell{materializedResult->GetValue(2, 0).GetValue<double>(), 0, true, ""});
      results.setData(3, n, table::TableCell{materializedResult->GetValue(3, 0).GetValue<double>(), 0, true, ""});
      results.setData(4, n, table::TableCell{materializedResult->GetValue(4, 0).GetValue<double>(), 0, true, ""});

    } catch(const duckdb::InternalException &) {
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toHeatmap(const QueryFilter &filter) -> joda::table::Table
{
  table::Table results;
  std::string controlImgPath;

  {
    std::unique_ptr<duckdb::QueryResult> images = filter.analyzer.select(
        "SELECT"
        "  images.image_id as image_id,"
        "  images.width as width,"
        "  images.height as height,"
        "  images.file_name as file_name "
        "FROM images "
        "WHERE"
        " image_id=$1",
        static_cast<uint64_t>(filter.imageId));
    if(images->HasError()) {
      throw std::invalid_argument("S:" + images->GetError());
    }

    auto imageMaterialized = images->Cast<duckdb::StreamQueryResult>().Materialize();
    uint64_t imgWidth      = imageMaterialized->GetValue(1, 0).GetValue<uint64_t>();
    uint64_t imgWeight     = imageMaterialized->GetValue(2, 0).GetValue<uint64_t>();
    controlImgPath         = imageMaterialized->GetValue(3, 0).GetValue<std::string>();

    std::string linkToImage = controlImgPath;
    helper::stringReplace(linkToImage, "${tile_id}", std::to_string(0));

    uint64_t width  = imgWidth / filter.densityMapAreaSize;
    uint64_t height = imgWeight / filter.densityMapAreaSize;

    for(uint64_t row = 0; row < height; row++) {
      results.getMutableRowHeader()[row] = std::to_string(row + 1);
      for(uint64_t col = 0; col < width; col++) {
        results.getMutableColHeader()[col] = std::to_string(col + 1);
        results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, linkToImage});
      }
    }
  }
  auto buildStats = [&]() {
    return getStatsString(filter.stats) + "(" + getMeasurement(filter.measurement) + ") as val";
  };

  {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer.select(
        "SELECT "
        "floor(meas_center_x / $3) * $3 AS rectangle_x,"
        "floor(meas_center_y / $3) * $3 AS rectangle_y," +
            buildStats() +
            " FROM objects "
            " WHERE"
            "  image_id=$1 AND cluster_id=$2 AND class_id=$4 "
            "GROUP BY floor(meas_center_x / $3), floor(meas_center_y / $3)",
        filter.imageId, static_cast<uint16_t>(filter.clusterId), duckdb::Value::DOUBLE(filter.densityMapAreaSize),
        static_cast<uint16_t>(filter.classId));

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      try {
        uint32_t rectangleX = materializedResult->GetValue(0, n).GetValue<double>();
        uint32_t rectangleY = materializedResult->GetValue(1, n).GetValue<double>();
        double value        = materializedResult->GetValue(2, n).GetValue<double>();

        uint32_t x = rectangleX / filter.densityMapAreaSize;
        uint32_t y = rectangleY / filter.densityMapAreaSize;

        std::string linkToImage = controlImgPath;
        results.setData(y, x, table::TableCell{value, 0, true, linkToImage});
      } catch(const duckdb::InternalException &) {
      }
    }
  }
  return results;
}

}    // namespace joda::db
