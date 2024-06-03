
#pragma once

#include <cstdint>
#include <string>
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/db_column_ids.hpp"
#include "helper.hpp"

namespace joda::results::analyze::plugins {

class HeatmapPerPlate
{
public:
  ///
  /// \brief      Get data for plate
  /// \author     Joachim Danmayr
  ///
  static auto getData(Analyzer &analyzer, uint8_t plateId, uint8_t plateRows, uint8_t plarteCols,
                      ChannelIndex channelId, const MeasureChannelId &measurement, Stats stats) -> Table
  {
    std::unique_ptr<duckdb::QueryResult> result = analyzer.getDatabase().select(
        "SELECT"
        "  image_group.group_id as group_id,"
        "  group.well_pos_x as x "
        "  group.well_pos_y as y " +
            getStatsString(stats) +
            "FROM object "
            "INNER JOIN image_group ON object.image_id=image_group.image_id "
            "INNER JOIN image ON object.image_id=image.image_id "
            "INNER JOIN channel_image ON (object.image_id=channel_image.image_id AND "
            "INNER JOIN group ON image_group.group_id=group.group_id "
            "object.channel_id=channel_image.channel_id) "
            "WHERE"
            " image_group.plate_id=$2 AND object.validity=0 AND channel_image.validity=0 AND "
            "object.channel_id=$3 "
            "GROUP BY"
            "  (image_group.group_id) ",
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
        if(row < plateRows && col < plarteCols) {
          double val = materializedResult->GetValue(3, n).GetValue<double>();
          results.setData(row, col, TableCell{val, groupId, true, ""});
        }
      } catch(const duckdb::InternalException &) {
      }
    }

    return results;
  }
};
}    // namespace joda::results::analyze::plugins
