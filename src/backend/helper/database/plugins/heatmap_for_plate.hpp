
#pragma once

#include <cstdint>
#include <string>
#include "../database.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/table/table.hpp"
#include "helper.hpp"

namespace joda::db {

class HeatmapPerPlate
{
public:
  ///
  /// \brief      Get data for plates
  /// \author     Joachim Danmayr
  ///
  static auto getData(Database &analyzer, uint8_t plateId, uint8_t plateRows, uint8_t plateCols,
                      enums::ClusterId clusterId, enums::ClassId classId, enums::Measurement measurement,
                      int32_t imageChannelId, enums::Stats stats) -> table::Table
  {
    auto buildStats = [&]() {
      return getStatsString(stats) + "(" + getMeasurement(measurement) + ") FILTER (images.validity = 0) as valid, " +
             getStatsString(stats) + "(" + getMeasurement(measurement) + ") FILTER (images.validity != 0) as invalid ";
    };

    auto queryMeasure = [&]() {
      std::unique_ptr<duckdb::QueryResult> result = analyzer.select(
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
          static_cast<uint16_t>(clusterId), static_cast<uint16_t>(classId));
      return result;
    };

    auto queryIntensityMeasure = [&]() {
      std::unique_ptr<duckdb::QueryResult> result = analyzer.select(
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
          static_cast<uint16_t>(clusterId), static_cast<uint16_t>(classId), static_cast<uint32_t>(imageChannelId));
      return result;
    };

    auto query = [&]() {
      switch(getType(measurement)) {
        case OBJECT:
          return queryMeasure();
        case INTENSITY:
          return queryIntensityMeasure();
        case COUNT:
          return queryMeasure();
      }
    };

    auto result = query();

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    table::Table results;

    for(uint8_t row = 0; row < plateRows; row++) {
      char toWrt[2];
      toWrt[0]                           = row + 'A';
      toWrt[1]                           = 0;
      results.getMutableRowHeader()[row] = std::string(toWrt);
      for(uint8_t col = 0; col < plateCols; col++) {
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

        if(row < plateRows && col < plateCols) {
          double val = 0;
          bool valid = true;
          if(!materializedResult->GetValue(3, n).IsNull()) {
            val = materializedResult->GetValue(3, n).GetValue<double>();
          }
          // At least one image in this well is invalid!
          if(!materializedResult->GetValue(4, n).IsNull()) {
            valid = false;
          }
          results.setData(row, col, table::TableCell{val, groupId, valid, ""});
        }
      } catch(const duckdb::InternalException &ex) {
      }
    }

    return results;
  }
};
}    // namespace joda::db
