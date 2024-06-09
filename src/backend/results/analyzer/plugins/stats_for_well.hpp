#pragma once

#include <cstdint>
#include "backend/results/analyzer/analyzer.hpp"

namespace joda::results::analyze::plugins {

class StatsPerGroup
{
public:
  ///
  /// \brief      Get data for group
  /// \author     Joachim Danmayr
  ///
  static auto getData(Analyzer &analyzer, uint8_t plateId, uint16_t groupId, ChannelIndex channelId,
                      const MeasureChannelId &measurement) -> Table
  {
    std::unique_ptr<duckdb::QueryResult> result = analyzer.getDatabase().select(
        "SELECT"
        "  objects.image_id as image_id,"
        "  images.file_name as file_name,"
        "  SUM(element_at(values, $1)[1]) as val_sum,"
        "  MIN(element_at(values, $1)[1]) as val_min,"
        "  MAX(element_at(values, $1)[1]) as val_max,"
        "  AVG(element_at(values, $1)[1]) as val_avg,"
        "  STDDEV(element_at(values, $1)[1]) as val_stddev "
        "FROM objects "
        "INNER JOIN images_groups ON objects.image_id=images_groups.image_id "
        "INNER JOIN images ON objects.image_id=images.image_id "
        "WHERE"
        " images_groups.group_id=$2 AND objects.validity=0 AND objects.channel_id=$3 "
        "GROUP BY"
        "  (objects.image_id, images.file_name) "
        "ORDER BY images.file_name",
        measurement.getKey(), groupId, static_cast<uint8_t>(channelId));

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    Table results;

    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      try {
        if(0 == n) {
          results.setColHeader({{0, measurement.toString() + "(sum)"},
                                {1, measurement.toString() + "(min)"},
                                {2, measurement.toString() + "(max)"},
                                {3, measurement.toString() + "(avg)"},
                                {4, measurement.toString() + "(stddev)"}});
        }

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
