
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
        "  image_well.well_id as well_id," +
            getStatsString(stats) +
            "FROM object "
            "INNER JOIN image_well ON object.image_id=image_well.image_id "
            "INNER JOIN image ON object.image_id=image.image_id "
            "WHERE"
            " image_well.plate_id=$2 AND validity=0 AND channel_id=$3 "
            "GROUP BY"
            "  (image_well.well_id) ",
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
        results.setData(row, col, TableCell{0, 0, false});
      }
    }

    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      try {
        WellId wellID{.well{.wellId = materializedResult->GetValue(0, n).GetValue<uint16_t>()}, .imageIdx = 0};
        uint8_t row = wellID.well.wellPos[WellId::POS_Y] - 1;
        uint8_t col = wellID.well.wellPos[WellId::POS_X] - 1;
        if(row < plateRows && col < plarteCols) {
          double val = materializedResult->GetValue(1, n).GetValue<double>();
          results.setData(row, col, TableCell{val, wellID.well.wellId, true});
        }
      } catch(const duckdb::InternalException &) {
      }
    }

    return results;
  }
};
}    // namespace joda::results::analyze::plugins
