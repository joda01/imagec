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

auto StatsPerPlate::toTable(const QueryFilter &filter) -> std::vector<joda::table::Table>
{
  auto queryResult = getData(filter);
  if(queryResult->HasError()) {
    throw std::invalid_argument(queryResult->GetError());
  }

  auto materializedResult = queryResult->Cast<duckdb::StreamQueryResult>().Materialize();
  table::Table results;
  results.getMutableColHeader()[0] = createHeader(filter);
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    try {
      uint16_t groupId      = materializedResult->GetValue(0, n).GetValue<uint16_t>();
      uint16_t col          = materializedResult->GetValue(1, n).GetValue<uint16_t>();
      uint16_t row          = materializedResult->GetValue(2, n).GetValue<uint16_t>();
      std::string imageName = materializedResult->GetValue(5, n).GetValue<std::string>();
      double valValid       = 0;
      bool valid;
      if(!materializedResult->GetValue(3, n).IsNull()) {
        valValid = materializedResult->GetValue(3, n).GetValue<double>();
      }
      // At least one image in this well is invalid!
      if(!materializedResult->GetValue(4, n).IsNull()) {
        if(materializedResult->GetValue(4, n).GetValue<double>() > 0) {
          valid = false;
        }
      }
      results.getMutableRowHeader()[n] = imageName;
      results.setData(n, 0, table::TableCell{valValid, groupId, true, ""});

    } catch(const duckdb::InternalException &ups) {
      std::cout << ups.what() << std::endl;
    }
  }

  return results;
}

auto StatsPerPlate::toHeatmap(const QueryFilter &filter) -> std::vector<joda::table::Table>
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

auto StatsPerPlate::getData(const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter)
    -> std::unique_ptr<duckdb::QueryResult>
{
  auto [sql, params] = toSQL(filter, channelFilter);
  return filter.analyzer->select(sql, params);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerPlate::toSQL(const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter)
    -> std::pair<std::string, DbArgs_t>
{
  auto buildStats = [&]() {
    return getStatsString(filter.stats) + "(" + getMeasurement(filter.measurementChannel) + ") FILTER (images.validity = 0) as valid, " +
           getStatsString(filter.stats) + "(" + getMeasurement(filter.measurementChannel) + ") FILTER (images.validity != 0) as invalid\n";
  };

  auto queryMeasure = [&]() {
    std::string sql =
        "SELECT\n"
        " subquery.group_id as groupid,\n"
        " ANY_VALUE(pos_on_plate_x) as pos_x,\n"
        " ANY_VALUE(pos_on_plate_y) as pos_y,\n"
        " AVG(valid) AS avg_valid,\n"
        " AVG(invalid) AS avg_invalid,\n"
        " ANY_VALUE(file_name) AS file_name\n"
        "FROM (\n"
        "     SELECT"
        "       objects.image_id,\n"
        "       ANY_VALUE(images.file_name) AS file_name,\n"
        "       images_groups.group_id as group_id,\n" +
        buildStats() +
        "     FROM objects\n"
        "     JOIN images ON objects.image_id = images.image_id\n"
        "     JOIN images_groups ON objects.image_id = images_groups.image_id\n"
        "     WHERE cluster_id = $1 AND class_id = $2\n"
        "     GROUP BY objects.image_id, images_groups.group_id\n"
        " ) AS subquery\n"
        " JOIN groups ON subquery.group_id = groups.group_id\n"
        " GROUP BY groupid";
    return sql;
  };

  auto queryIntensityMeasure = [&]() {
    std::string sql =
        "SELECT\n"
        " subquery.group_id as groupid,\n"
        " ANY_VALUE(pos_on_plate_x) as pos_x,\n"
        " ANY_VALUE(pos_on_plate_y) as pos_y,\n"
        " AVG(valid) AS avg_valid,\n"
        " AVG(invalid) AS avg_invalid,\n"
        " ANY_VALUE(file_name) AS file_name\n"
        " FROM (\n"
        "     SELECT\n"
        "         objects.image_id,\n"
        "         ANY_VALUE(images.file_name) AS file_name,\n"
        "         images_groups.group_id as group_id,\n" +
        buildStats() +
        "     FROM objects\n"
        "     JOIN images ON objects.image_id = images.image_id\n"
        "     JOIN images_groups ON objects.image_id = images_groups.image_id\n"
        "     JOIN object_measurements ON (objects.object_id = object_measurements.object_id AND\n"
        "                                  objects.image_id = object_measurements.image_id)\n"
        "     WHERE cluster_id = $1 AND class_id = $2 AND object_measurements.meas_stack_c = $3\n"
        "     GROUP BY objects.image_id, images_groups.group_id\n"
        " ) AS subquery\n"
        " JOIN groups ON subquery.group_id = groups.group_id\n"
        " GROUP BY groupid";
    return sql;
  };

  switch(getType(filter.measurementChannel)) {
    default:
    case OBJECT:
      return {queryMeasure(), {static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId)}};
    case INTENSITY:
      return {queryIntensityMeasure(),
              {static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId), static_cast<uint32_t>(filter.crossChanelStack_c)}};
  }
}

}    // namespace joda::db
