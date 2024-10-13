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
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/plugins/helper.hpp"

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
  if(filter.measurementChannel == enums::Measurement::COUNT) {
    return {};
  }
  auto buildStats = [&]() { return getMeasurement(filter.measurementChannel) + " as val"; };

  auto queryMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> stats =
        filter.analyzer->select("SELECT " + buildStats() +
                                    " FROM objects "
                                    "WHERE"
                                    " objects.image_id=$1 AND objects.cluster_id=$2 AND objects.class_id=$3 ",
                                filter.actImageId, static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId));
    return stats;
  };

  auto queryIntensityMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> stats = filter.analyzer->select(
        "SELECT " + buildStats() +
            " FROM objects "
            "JOIN object_measurements ON (objects.object_id = object_measurements.object_id AND "
            "                             objects.image_id = object_measurements.image_id AND  "
            "                             object_measurements.meas_stack_c = $4 )"
            " WHERE"
            "  objects.image_id=$1 AND objects.cluster_id=$2 AND objects.class_id=$3 AND "
            "object_measurements.meas_stack_c = $4",
        filter.actImageId, static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId), filter.crossChanelStack_c);
    return stats;
  };

  auto query = [&]() -> std::unique_ptr<duckdb::QueryResult> {
    switch(getType(filter.measurementChannel)) {
      default:
      case OBJECT:
        return queryMeasure();
      case INTENSITY:
        return queryIntensityMeasure();
    }
  };

  auto result = query();

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
  table::Table results;
  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  results.setColHeader({{0, createHeader(filter)}}    // namespace joda::db
  );

  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    results.getMutableRowHeader()[n] = std::to_string(n);
    try {
      results.setData(n, 0, table::TableCell{materializedResult->GetValue(0, n).GetValue<double>(), 0, true, ""});
    } catch(const duckdb::InternalException &) {
    }
  }
  return results;
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
  auto [result, imageInfo] = densityMap(filter);

  table::Table results;

  for(uint64_t row = 0; row < imageInfo.height; row++) {
    results.getMutableRowHeader()[row] = std::to_string(row + 1);
    for(uint64_t col = 0; col < imageInfo.width; col++) {
      results.getMutableColHeader()[col] = std::to_string(col + 1);
      results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, imageInfo.controlImgPath});
    }
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    try {
      uint32_t rectangleX = materializedResult->GetValue(0, n).GetValue<double>();
      uint32_t rectangleY = materializedResult->GetValue(1, n).GetValue<double>();
      double value        = materializedResult->GetValue(2, n).GetValue<double>();

      uint32_t x = rectangleX / filter.densityMapAreaSize;
      uint32_t y = rectangleY / filter.densityMapAreaSize;

      std::string linkToImage = imageInfo.controlImgPath;
      results.setData(y, x, table::TableCell{value, 0, true, linkToImage});
    } catch(const duckdb::InternalException &ex) {
      std::cout << "EX " << ex.what() << std::endl;
    }
  }
  return results;
}

///
/// \brief          Returns the heatmap in list form
/// \author         Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toHeatmapList(const QueryFilter &filter) -> joda::table::Table
{
  auto [result, imageInfo] = densityMap(filter);

  table::Table results;

  std::map<uint32_t, std::pair<double, std::string>> sortedData;

  results.getMutableColHeader()[0] = createHeader(filter);
  int tableRowCount                = 0;
  for(uint16_t row = 0; row < imageInfo.height; row++) {
    for(uint16_t col = 0; col < imageInfo.width; col++) {
      uint32_t key                                 = ((row << 16) & 0xFFFF0000) | (col & 0xFFFF);
      char letter                                  = 'A' + row;
      results.getMutableRowHeader()[tableRowCount] = std::string(1, letter) + "" + std::to_string(col + 1);
      results.setData(tableRowCount, 0, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, imageInfo.controlImgPath});
      sortedData.emplace(key, std::pair<double, std::string>{std::numeric_limits<double>::quiet_NaN(), ""});
      tableRowCount++;
    }
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    try {
      uint32_t rectangleX = materializedResult->GetValue(0, n).GetValue<double>();
      uint32_t rectangleY = materializedResult->GetValue(1, n).GetValue<double>();
      double value        = materializedResult->GetValue(2, n).GetValue<double>();

      uint32_t x   = rectangleX / filter.densityMapAreaSize;
      uint32_t y   = rectangleY / filter.densityMapAreaSize;
      uint32_t key = ((y << 16) & 0xFFFF0000) | (x & 0xFFFF);

      std::string linkToImage = imageInfo.controlImgPath;
      sortedData[key]         = {value, linkToImage};
    } catch(const duckdb::InternalException &ex) {
      std::cout << "EX " << ex.what() << std::endl;
    }
  }

  tableRowCount = 0;
  for(const auto &[key, data] : sortedData) {
    uint16_t row                                 = (key >> 16) & 0xFFFF;
    uint16_t col                                 = (key >> 0) & 0xFFFF;
    char letter                                  = 'A' + row;
    results.getMutableRowHeader()[tableRowCount] = std::string(1, letter) + "" + std::to_string(col + 1);
    results.setData(tableRowCount, 0, table::TableCell{std::get<0>(data), 0, true, std::get<1>(data)});
    tableRowCount++;
  }
  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::densityMap(const QueryFilter &filter) -> std::tuple<std::unique_ptr<duckdb::QueryResult>, ImgInfo>
{
  std::string controlImgPath;
  ImgInfo imgInfo;

  {
    std::unique_ptr<duckdb::QueryResult> images = filter.analyzer->select(
        "SELECT"
        "  images.image_id as image_id,"
        "  images.width as width,"
        "  images.height as height,"
        "  images.file_name as file_name "
        "FROM images "
        "WHERE"
        " image_id=$1",
        static_cast<uint64_t>(filter.actImageId));
    if(images->HasError()) {
      throw std::invalid_argument("S:" + images->GetError());
    }

    auto imageMaterialized = images->Cast<duckdb::StreamQueryResult>().Materialize();
    uint64_t imgWidth      = imageMaterialized->GetValue(1, 0).GetValue<uint64_t>();
    uint64_t imgWeight     = imageMaterialized->GetValue(2, 0).GetValue<uint64_t>();
    controlImgPath         = imageMaterialized->GetValue(3, 0).GetValue<std::string>();

    std::string linkToImage = controlImgPath;
    helper::stringReplace(linkToImage, "${tile_id}", std::to_string(0));

    uint64_t width  = std::ceil((float) imgWidth / (float) filter.densityMapAreaSize);
    uint64_t height = std::ceil((float) imgWeight / (float) filter.densityMapAreaSize);

    imgInfo.width          = width;
    imgInfo.height         = height;
    imgInfo.controlImgPath = linkToImage;
  }
  auto buildStats = [&]() { return getStatsString(filter.stats) + "(" + getMeasurement(filter.measurementChannel) + ") as val"; };

  auto queryMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer->select(
        "SELECT "
        "floor(meas_center_x / $4) * $4 AS rectangle_x,"
        "floor(meas_center_y / $4) * $4 AS rectangle_y," +
            buildStats() +
            " FROM objects "
            " WHERE"
            "  image_id=$1 AND cluster_id=$2 AND class_id=$3 "
            "GROUP BY floor(meas_center_x / $4), floor(meas_center_y / $4)",
        filter.actImageId, static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId),
        duckdb::Value::DOUBLE(filter.densityMapAreaSize));

    return result;
  };

  auto queryIntensityMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer->select(
        "SELECT "
        "floor(meas_center_x / $4) * $4 AS rectangle_x,"
        "floor(meas_center_y / $4) * $4 AS rectangle_y," +
            buildStats() +
            " FROM objects "
            "JOIN object_measurements ON (objects.object_id = object_measurements.object_id AND "
            "                                  objects.image_id = object_measurements.image_id "
            "                             AND object_measurements.meas_stack_c = $5)"
            " WHERE"
            "  objects.image_id=$1 AND cluster_id=$2 AND class_id=$3 "
            "GROUP BY floor(meas_center_x / $4), floor(meas_center_y / $4)",
        filter.actImageId, static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId),
        duckdb::Value::DOUBLE(filter.densityMapAreaSize), filter.crossChanelStack_c);

    return result;
  };

  auto query = [&]() {
    switch(getType(filter.measurementChannel)) {
      default:
      case OBJECT:
        return queryMeasure();
      case INTENSITY:
        return queryIntensityMeasure();
    }
  };

  std::unique_ptr<duckdb::QueryResult> result = query();

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  return {std::move(result), imgInfo};
}

}    // namespace joda::db
