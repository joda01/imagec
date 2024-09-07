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
    std::unique_ptr<duckdb::QueryResult> stats = filter.analyzer->select(
        "SELECT " + buildStats() +
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
        filter.actImageId, static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId),
        filter.crossChanelStack_c);
    return stats;
  };

  auto queryIntersectingMeasure = [&]() {
    std::cout << "Cross channel " << std::to_string((uint16_t) filter.clusterId) << " | "
              << std::to_string((uint16_t) filter.crossChannelClusterId) << std::endl;

    std::unique_ptr<duckdb::QueryResult> stats = filter.analyzer->select(
        "SELECT "
        "COUNT(inners.meas_object_id) "
        "FROM objects "
        "JOIN "
        "("
        "	SELECT intersect_in.object_id, intersect_in.meas_object_id FROM objects "
        "	JOIN object_intersections AS intersect_in ON  objects.object_id = intersect_in.meas_object_id "
        "	WHERE cluster_id = $4 AND class_id = $5 AND objects.image_id = $3 "
        ") as inners "
        "   on objects.object_id = inners.object_id "
        "   WHERE objects.cluster_id = $1 AND objects.class_id = $2 AND objects.image_id = $3 "
        "   GROUP BY objects.object_id",
        static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId), filter.actImageId,
        static_cast<uint16_t>(filter.crossChannelClusterId), static_cast<uint16_t>(filter.crossChannelClassId));
    return stats;
  };

  auto query = [&]() -> std::unique_ptr<duckdb::QueryResult> {
    switch(getType(filter.measurementChannel)) {
      default:
      case OBJECT:
        return queryMeasure();
      case INTENSITY:
        return queryIntensityMeasure();
      case COUNT:
        return queryIntersectingMeasure();
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
  table::Table results;
  std::string controlImgPath;

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
    return getStatsString(filter.stats) + "(" + getMeasurement(filter.measurementChannel) + ") as val";
  };

  {
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

    auto queryIntersectingMeasure = [&]() {
      std::unique_ptr<duckdb::QueryResult> result = filter.analyzer->select(
          "SELECT "
          "floor(meas_center_x / $4) * $4 AS rectangle_x,"
          "floor(meas_center_y / $4) * $4 AS rectangle_y,"
          "COUNT(inners.meas_object_id) FILTER (images.validity = 0) as valid,"
          "COUNT(inners.meas_object_id) FILTER (images.validity != 0) as invalid "
          "FROM objects "
          "JOIN "
          "("
          "	SELECT intersect_in.object_id, intersect_in.meas_object_id FROM objects "
          "	JOIN object_intersections AS intersect_in ON  objects.object_id = intersect_in.meas_object_id "
          "	WHERE cluster_id = $5 AND class_id = $6 AND objects.image_id = $3 "
          ") as inners "
          "on objects.object_id = inners.object_id "
          "JOIN images on objects.image_id = images.image_id "
          "WHERE objects.cluster_id = $1 AND objects.class_id = $2 AND objects.image_id = $3 "
          "GROUP BY objects.object_id, floor(meas_center_x / $4), floor(meas_center_y / $4)",
          static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId), filter.actImageId,
          duckdb::Value::DOUBLE(filter.densityMapAreaSize), static_cast<uint16_t>(filter.crossChannelClusterId),
          static_cast<uint16_t>(filter.crossChannelClassId));

      return result;
    };

    auto query = [&]() {
      switch(getType(filter.measurementChannel)) {
        default:
        case OBJECT:
          return queryMeasure();
        case INTENSITY:
          return queryIntensityMeasure();
        case COUNT:
          return queryIntersectingMeasure();
      }
    };

    auto result = query();

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
      } catch(const duckdb::InternalException &ex) {
        std::cout << "EX " << ex.what() << std::endl;
      }
    }
  }
  return results;
}

}    // namespace joda::db
