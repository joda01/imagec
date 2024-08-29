#pragma once

#include <cstdint>
#include "../database.hpp"
#include "backend/helper/database/plugins/helper.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

class StatsPerGroup
{
public:
  static auto toTable(const QueryFilter &filter) -> joda::table::Table;
  static auto toHeatmap(const QueryFilter &filter) -> joda::table::Table;

private:
  static auto getData(const QueryFilter &filter) -> std::unique_ptr<duckdb::QueryResult>;

  ///
  /// \brief      Get data for group
  /// \author     Joachim Danmayr
  ///
  static auto getData(Database &analyzer, uint8_t plateId, uint16_t groupId, enums::ClusterId clusterId,
                      enums::ClassId classId) -> table::Table
  {
    return {};
    /*
    std::unique_ptr<duckdb::QueryResult> result = analyzer.select(
        "SELECT"
        "  objects.image_id as image_id,"
        "  ANY_VALUE(images.file_name as file_name),"
        "  AVG(sum, $1) as val_sum,"
        "  AVG(min, $1) as val_min,"
        "  AVG(max, $1) as val_max,"
        "  AVG(avg, $1) as val_avg,"
        "  AVG(stddev, $1) as val_stddev,"
        "  AVG(cnt, $1) as val_cnt "
        "FROM objects "
        "INNER JOIN images_groups ON objects.image_id=images_groups.image_id "
        "INNER JOIN images ON objects.image_id=images.image_id "
        "INNER JOIN images ON objects.image_id=object_measurements.image_id AND objects.object_id = "
        "object_measurements.object_id "
        "WHERE"
        " images_groups.group_id=$1 AND objects.cluster_id=$2 AND objects.class_id=$3"
        "GROUP BY image_id "
        "ORDER BY images.file_name",
        groupId, static_cast<uint8_t>(clusterId), static_cast<uint8_t>(classId));

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    table::Table results;

    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      try {
        if(0 == n) {
          results.setColHeader({{0, toString(measurement) + "(sum)"},
                                {1, toString(measurement) + "(min)"},
                                {2, toString(measurement) + "(max)"},
                                {3, toString(measurement) + "(avg)"},
                                {4, toString(measurement) + "(stddev)"},
                                {5, toString(measurement) + "(cnt)"}});
        }

        uint64_t id = materializedResult->GetValue(0, n).GetValue<uint64_t>();

        // results.data()[n][0]             = TableCell{materializedResult->GetValue(0, n).GetValue<uint64_t>(), true};
        results.getMutableRowHeader()[n] = materializedResult->GetValue(1, n).GetValue<std::string>(), true;
        results.setData(n, 0, table::TableCell{materializedResult->GetValue(2, n).GetValue<double>(), id, true, ""});
        results.setData(n, 1, table::TableCell{materializedResult->GetValue(3, n).GetValue<double>(), id, true, ""});
        results.setData(n, 2, table::TableCell{materializedResult->GetValue(4, n).GetValue<double>(), id, true, ""});
        results.setData(n, 3, table::TableCell{materializedResult->GetValue(5, n).GetValue<double>(), id, true, ""});
        results.setData(n, 4, table::TableCell{materializedResult->GetValue(6, n).GetValue<double>(), id, true, ""});
        results.setData(n, 5, table::TableCell{materializedResult->GetValue(7, n).GetValue<double>(), id, true, ""});
      } catch(const duckdb::InternalException &) {
      }
    }

    results.print();
    return results;
    */
  }
};
}    // namespace joda::db
