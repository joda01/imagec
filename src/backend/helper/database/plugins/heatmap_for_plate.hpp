
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
  static auto getData(Database &analyzer, uint8_t plateId, uint8_t plateRows, uint8_t plarteCols,
                      enums::ClusterId channelId, enums::ClassId classId, enums::Measurement stats) -> table::Table
  {
    return {};
    /*
    std::unique_ptr<duckdb::QueryResult> result = analyzer.getDatabase().select(
        "SELECT"
        "	images_groups.group_id as group_id,"
        "	ANY_VALUE(groups.well_pos_x) as wx,"
        "	ANY_VALUE(groups.well_pos_y) as wy," +
            getAvgStatsFromStats(stats) +
            "FROM"
            "	image_stats "
            "INNER JOIN images_groups ON"
            "	image_stats.image_id = images_groups.image_id "
            "INNER JOIN images ON"
            "	image_stats.image_id = images.image_id "
            "INNER JOIN channels_images ON"
            "	(image_stats.image_id = channels_images.image_id AND image_stats.channel_id = "
            "channels_images.channel_id) "
            "INNER JOIN groups ON"
            "	images_groups.group_id = groups.group_id "
            "WHERE"
            "	images_groups.plate_id = $2 "
            "	AND channels_images.validity = 0 "
            "	AND image_stats.channel_id = $3 "
            "GROUP BY"
            "	(images_groups.group_id);",
        measurement.getKey(), plateId, static_cast<uint8_t>(channelId));

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    Table results;

    for(uint8_t row = 0; row < plateRows; row++) {
      char toWrt[2];
      toWrt[0]                           = row + 'A';
      toWrt[1]                           = 0;
      results.getMutableRowHeader()[row] = std::string(toWrt);
      for(uint8_t col = 0; col < plarteCols; col++) {
        results.getMutableColHeader()[col] = std::to_string(col + 1);
        results.setData(row, col, TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, ""});
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

        if(row < plateRows && col < plarteCols) {
          double val = materializedResult->GetValue(3, n).GetValue<double>();
          results.setData(row, col, TableCell{val, groupId, true, ""});
        }
      } catch(const duckdb::InternalException &ex) {
      }
    }

    return results;*/
  }
};
}    // namespace joda::db
