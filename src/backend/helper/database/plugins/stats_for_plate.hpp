
#pragma once

#include <cstdint>
#include <string>
#include "../database.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

class StatsPerPlate
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
    std::unique_ptr<duckdb::QueryResult> result = analyzer.select(
        "SELECT"
        "	images_groups.group_id as group_id,"
        "	ANY_VALUE(groups.well_pos_x) as wx,"
        "	ANY_VALUE(groups.well_pos_y) as wy,"
        "	ANY_VALUE(groups.name) as group_name," +
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
        classId, plateId, static_cast<uint8_t>(channelId));

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    table::Table results;

    results.getMutableColHeader()[0] = measurement.toString();
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      try {
        uint16_t groupId                 = materializedResult->GetValue(0, n).GetValue<uint16_t>();
        uint16_t col                     = materializedResult->GetValue(1, n).GetValue<uint16_t>();
        uint16_t row                     = materializedResult->GetValue(2, n).GetValue<uint16_t>();
        std::string groupName            = materializedResult->GetValue(3, n).GetValue<std::string>();
        double val                       = materializedResult->GetValue(4, n).GetValue<double>();
        results.getMutableRowHeader()[n] = groupName;
        results.setData(n, 0, table::TableCell{val, groupId, true, ""});

      } catch(const duckdb::InternalException &ups) {
        std::cout << ups.what() << std::endl;
      }
    }

    return results;*/
  }
};
}    // namespace joda::db
