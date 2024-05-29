#pragma once

#include <cstdint>
#include "backend/results/analyzer/analyzer.hpp"

namespace joda::results::analyze::plugins {

class StatsPerWell
{
public:
  ///
  /// \brief      Get data for well
  /// \author     Joachim Danmayr
  ///
  static auto getData(Analyzer &analyzer, uint8_t plateId, results::WellId wellId, ChannelIndex channelId,
                      const MeasureChannelId &measurement) -> Table
  {
    std::unique_ptr<duckdb::QueryResult> result = analyzer.getDatabase().select(
        "SELECT"
        "  object.image_id as image_id,"
        "  image.file_name as file_name,"
        "  SUM(element_at(values, $1)[1]) as val_sum,"
        "  MIN(element_at(values, $1)[1]) as val_min,"
        "  MAX(element_at(values, $1)[1]) as val_max,"
        "  AVG(element_at(values, $1)[1]) as val_avg,"
        "  STDDEV(element_at(values, $1)[1]) as val_stddev "
        "FROM object "
        "INNER JOIN image_well ON object.image_id=image_well.image_id "
        "INNER JOIN image ON object.image_id=image.image_id "
        "WHERE"
        " image_well.well_id=$2 AND bit_count(object.validity)=0 AND object.channel_id=$3 "
        "GROUP BY"
        "  (object.image_id, image.file_name) "
        "ORDER BY image.file_name",
        measurement.getKey(), wellId.well.wellId, static_cast<uint8_t>(channelId));

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    Table results;
    results.setColHeader({{0, "sum"}, {1, "min"}, {2, "max"}, {3, "avg"}, {4, "stddev"}});
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      try {
        uint64_t id = materializedResult->GetValue(0, n).GetValue<uint64_t>();

        // results.data()[n][0]             = TableCell{materializedResult->GetValue(0, n).GetValue<uint64_t>(), true};
        results.getMutableRowHeader()[n] = materializedResult->GetValue(1, n).GetValue<std::string>(), true;
        results.setData(n, 0, TableCell{materializedResult->GetValue(2, n).GetValue<double>(), id, true, ""});
        results.setData(n, 1, TableCell{materializedResult->GetValue(3, n).GetValue<double>(), id, true, ""});
        results.setData(n, 2, TableCell{materializedResult->GetValue(4, n).GetValue<double>(), id, true, ""});
        results.setData(n, 3, TableCell{materializedResult->GetValue(5, n).GetValue<double>(), id, true, ""});
        results.setData(n, 4, TableCell{materializedResult->GetValue(6, n).GetValue<double>(), id, true, ""});
      } catch(const duckdb::InternalException &) {
      }
    }

    results.print();
    return results;
  }
};
}    // namespace joda::results::analyze::plugins
