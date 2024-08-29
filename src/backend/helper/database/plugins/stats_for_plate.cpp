///
/// \file      stats_for_plate.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "stats_for_plate.hpp"

namespace joda::db {

auto StatsPerPlate::getData(const Filter &filter) -> std::unique_ptr<duckdb::QueryResult>
{
  auto buildStats = [&]() {
    return getStatsString(filter.stats) + "(" + getMeasurement(filter.measurement) +
           ") FILTER (images.validity = 0) as valid, " + getStatsString(filter.stats) + "(" +
           getMeasurement(filter.measurement) + ") FILTER (images.validity != 0) as invalid ";
  };

  auto queryMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer.select(
        "SELECT"
        " subquery.group_id as groupid,"
        " ANY_VALUE(pos_on_plate_x) as pos_x,"
        " ANY_VALUE(pos_on_plate_y) as pos_y,"
        " AVG(valid) AS avg_valid,"
        " AVG(invalid) AS avg_invalid"
        " FROM ("
        "     SELECT"
        "         objects.image_id,"
        "         images_groups.group_id as group_id," +
            buildStats() +
            "     FROM objects "
            "     JOIN images ON objects.image_id = images.image_id "
            "     JOIN images_groups ON objects.image_id = images_groups.image_id "
            "     WHERE cluster_id = $1 AND class_id = $2"
            "     GROUP BY objects.image_id, images_groups.group_id"
            " ) AS subquery"
            " JOIN groups ON subquery.group_id = groups.group_id "
            " GROUP BY groupid",
        static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId));
    return result;
  };

  auto queryIntensityMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer.select(
        "SELECT"
        " subquery.group_id as groupid,"
        " ANY_VALUE(pos_on_plate_x) as pos_x,"
        " ANY_VALUE(pos_on_plate_y) as pos_y,"
        " AVG(valid) AS avg_valid,"
        " AVG(invalid) AS avg_invalid"
        " FROM ("
        "     SELECT"
        "         objects.image_id,"
        "         images_groups.group_id as group_id," +
            buildStats() +
            "     FROM objects "
            "     JOIN images ON objects.image_id = images.image_id "
            "     JOIN images_groups ON objects.image_id = images_groups.image_id "
            "     JOIN object_measurements ON (objects.object_id = object_measurements.object_id AND "
            "                                  objects.image_id = object_measurements.image_id)"
            "     WHERE cluster_id = $1 AND class_id = $2 AND object_measurements.meas_stack_c = $3"
            "     GROUP BY objects.image_id, images_groups.group_id"
            " ) AS subquery"
            " JOIN groups ON subquery.group_id = groups.group_id "
            " GROUP BY groupid",
        static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId),
        static_cast<uint32_t>(filter.imageChannelId));
    return result;
  };

  auto queryIntersectingMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer.select(
        "SELECT"
        " subquery.group_id as groupid,"
        " ANY_VALUE(pos_on_plate_x) as pos_x,"
        " ANY_VALUE(pos_on_plate_y) as pos_y,"
        " AVG(valid) AS avg_valid,"
        " AVG(invalid) AS avg_invalid"
        " FROM ("
        "     SELECT"
        "       object_intersections.image_id,"
        "     	objects.object_id,"
        "       images_groups.group_id as group_id,"
        "     	COUNT(object_intersections.meas_object_id) FILTER (images.validity = 0) as valid,"
        "     	COUNT(object_intersections.meas_object_id) FILTER (images.validity != 0) as invalid"
        "     FROM"
        "     	object_intersections"
        "   	JOIN images ON object_intersections.image_id = images.image_id"
        "     JOIN images_groups ON object_intersections.image_id = images_groups.image_id "
        "     JOIN objects ON"
        "     	objects.object_id = object_intersections.meas_object_id"
        "     	AND objects.image_id = object_intersections.image_id   "
        "     	AND objects.cluster_id = $1                            "
        "     	AND objects.class_id = $2                              "
        "     WHERE objects.cluster_id = $1 AND objects.class_id = $2  "
        "     GROUP BY                                                 "
        "     	(objects.object_id, object_intersections.image_id, images_groups.group_id)     "
        "  ) AS subquery"
        " JOIN groups ON subquery.group_id = groups.group_id "
        " GROUP BY groupid",
        static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId));

    return result;
  };

  auto query = [&]() {
    switch(getType(filter.measurement)) {
      case OBJECT:
        return queryMeasure();
      case INTENSITY:
        return queryIntensityMeasure();
      case COUNT:
        return queryIntersectingMeasure();
    }
  };

  return query();
}

auto StatsPerPlate::toTable(const Filter &filter) -> joda::table::Table
{
  auto queryResult = getData(filter);
  if(queryResult->HasError()) {
    throw std::invalid_argument(queryResult->GetError());
  }
  auto materializedResult = queryResult->Cast<duckdb::StreamQueryResult>().Materialize();
  table::Table results;
  results.getMutableColHeader()[0] = toString(filter.measurement) + "(" + filter.className + ")";
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    try {
      uint16_t groupId      = materializedResult->GetValue(0, n).GetValue<uint16_t>();
      uint16_t col          = materializedResult->GetValue(1, n).GetValue<uint16_t>();
      uint16_t row          = materializedResult->GetValue(2, n).GetValue<uint16_t>();
      std::string groupName = materializedResult->GetValue(3, n).GetValue<std::string>();
      double valValid       = 0;
      if(!materializedResult->GetValue(4, n).IsNull()) {
        valValid = materializedResult->GetValue(4, n).GetValue<double>();
      }
      results.getMutableRowHeader()[n] = groupName;
      results.setData(n, 0, table::TableCell{valValid, groupId, true, ""});

    } catch(const duckdb::InternalException &ups) {
      std::cout << ups.what() << std::endl;
    }
  }

  return results;
}

auto StatsPerPlate::toHeatmap(const Filter &filter) -> joda::table::Table
{
  auto queryResult = getData(filter);
  if(queryResult->HasError()) {
    throw std::invalid_argument(queryResult->GetError());
  }
  auto materializedResult = queryResult->Cast<duckdb::StreamQueryResult>().Materialize();

  table::Table results;
  for(uint8_t row = 0; row < filter.plateRows; row++) {
    char toWrt[2];
    toWrt[0]                           = row + 'A';
    toWrt[1]                           = 0;
    results.getMutableRowHeader()[row] = std::string(toWrt);
    for(uint8_t col = 0; col < filter.plateCols; col++) {
      results.getMutableColHeader()[col] = std::to_string(col + 1);
      results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, ""});
    }
  }

  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    try {
      uint16_t groupId = materializedResult->GetValue(0, n).GetValue<uint16_t>();
      uint16_t col     = materializedResult->GetValue(1, n).GetValue<uint16_t>();
      uint16_t row     = materializedResult->GetValue(2, n).GetValue<uint16_t>();
      if(col > 0) {
        col--;
      }
      if(row > 0) {
        row--;
      }

      if(row < filter.plateRows && col < filter.plateCols) {
        double val = 0;
        bool valid = true;
        if(!materializedResult->GetValue(3, n).IsNull()) {
          val = materializedResult->GetValue(3, n).GetValue<double>();
        }
        // At least one image in this well is invalid!
        if(!materializedResult->GetValue(4, n).IsNull()) {
          if(materializedResult->GetValue(4, n).GetValue<double>() > 0) {
            valid = false;
          }
        }
        results.setData(row, col, table::TableCell{val, groupId, valid, ""});
      }
    } catch(const duckdb::InternalException &ex) {
    }
  }

  return results;
}
}    // namespace joda::db
