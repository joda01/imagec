#pragma once

#include <cstdint>
#include <string>
#include "backend/results/analyzer/analyzer.hpp"

namespace joda::results::analyze::plugins {

class StatsPerImage
{
public:
  ///
  /// \brief      Get data for well
  /// \author     Joachim Danmayr
  ///
  static auto getData(Analyzer &analyzer, uint8_t plateId, uint64_t imageId, ChannelIndex channelId,
                      const MeasureChannelId &measurement) -> Table
  {
    Table results;

    // Stats
    {
      std::unique_ptr<duckdb::QueryResult> stats = analyzer.getDatabase().select(
          "SELECT"
          "  SUM(element_at(values, $1)[1]) as val_sum,"
          "  MIN(element_at(values, $1)[1]) as val_min,"
          "  MAX(element_at(values, $1)[1]) as val_max,"
          "  AVG(element_at(values, $1)[1]) as val_avg,"
          "  STDDEV(element_at(values, $1)[1]) as val_stddev "
          "FROM objects "
          "WHERE"
          " objects.image_id=$2 AND objects.validity=0 AND objects.channel_id=$3 ",
          measurement.getKey(), imageId, static_cast<uint8_t>(channelId));

      if(stats->HasError()) {
        throw std::invalid_argument(stats->GetError());
      }

      auto materializedResult = stats->Cast<duckdb::StreamQueryResult>().Materialize();
      results.setColHeader({{0, measurement.toString()}});

      results.getMutableRowHeader()[0] = "sum";
      results.getMutableRowHeader()[1] = "min";
      results.getMutableRowHeader()[2] = "max";
      results.getMutableRowHeader()[3] = "avg";
      results.getMutableRowHeader()[4] = "stddev";
      for(size_t n = 0; n < materializedResult->RowCount(); n++) {
        try {
          results.setData(0, n, TableCell{materializedResult->GetValue(0, 0).GetValue<double>(), 0, true, ""});
          results.setData(1, n, TableCell{materializedResult->GetValue(1, 0).GetValue<double>(), 0, true, ""});
          results.setData(2, n, TableCell{materializedResult->GetValue(2, 0).GetValue<double>(), 0, true, ""});
          results.setData(3, n, TableCell{materializedResult->GetValue(3, 0).GetValue<double>(), 0, true, ""});
          results.setData(4, n, TableCell{materializedResult->GetValue(4, 0).GetValue<double>(), 0, true, ""});

        } catch(const duckdb::InternalException &) {
        }
      }
    }

    // Detail data
    {
      std::unique_ptr<duckdb::QueryResult> stats = analyzer.getDatabase().select(
          "SELECT"
          "  objects.object_id as object_id,"
          "  element_at(values, $1)[1] as val "
          "FROM objects "
          "WHERE"
          " objects.image_id=$2 AND objects.validity=0 AND objects.channel_id=$3 "
          "ORDER BY objects.object_id",
          measurement.getKey(), imageId, static_cast<uint8_t>(channelId));

      if(stats->HasError()) {
        throw std::invalid_argument(stats->GetError());
      }

      auto materializedResult = stats->Cast<duckdb::StreamQueryResult>().Materialize();
      for(size_t n = 0; n < materializedResult->RowCount(); n++) {
        try {
          uint64_t id = materializedResult->GetValue(0, n).GetValue<uint64_t>();
          results.getMutableRowHeader()[n + 6] =
              std::to_string(materializedResult->GetValue(0, n).GetValue<uint64_t>()),
                                            true;
          results.setData(n + 6, 0, TableCell{materializedResult->GetValue(0, n).GetValue<double>(), id, true, ""});

        } catch(const duckdb::InternalException &) {
        }
      }
    }

    results.print();
    return results;
  }
};
}    // namespace joda::results::analyze::plugins
