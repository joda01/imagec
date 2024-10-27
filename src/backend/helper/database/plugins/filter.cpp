///
/// \file      filter.cpp
/// \author    Joachim Danmayr
/// \date      2024-10-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "filter.hpp"
#include <string>
#include "backend/enums/enum_measurements.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto joda::db::QueryFilter::getClustersAndClassesToExport() const -> ResultingTable
{
  return ResultingTable{this};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
ResultingTable::ResultingTable(const QueryFilter *filter)
{
  std::map<int32_t, std::map<uint32_t, std::string>> tableHeaders;
  for(const auto &[colIdx, colKey] : filter->getColumns()) {
    QueryKey qKey = {colKey.clusterClass, colKey.zStack, colKey.tStack};
    if(!mClustersAndClasses.contains(qKey)) {
      mClustersAndClasses.emplace(qKey, PreparedStatement{colKey.names});
    }
    mClustersAndClasses.at(qKey).addColumn(colKey);
    mTableMapping.emplace(colKey, colIdx);
    tableHeaders[colIdx.tabIdx].emplace(colIdx.colIdx, colKey.createHeader());
  }

  for(const auto &[tabIdx, header] : tableHeaders) {
    mResultingTable[tabIdx].setColHeader(header);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::string PreparedStatement::createStatsQuery(bool isOuter, bool excludeInvalid, std::optional<enums::Stats> overrideStats) const
{
  std::string channels;
  for(const auto &[_, column] : columns) {
    auto createName = [&column = column, &isOuter]() -> std::string {
      if(!isOuter) {
        return getMeasurement(column.measureChannel, false);
      } else {
        return getMeasurement(column.measureChannel, true) + "_" + getStatsString(column.stats);
      }
    };

    auto stats = overrideStats.has_value() ? overrideStats.value() : column.stats;

    auto injectCase = [&excludeInvalid](const std::string &columnToCalc) {
      if(excludeInvalid) {
        return "CASE WHEN validity = 0 THEN " + columnToCalc + " ELSE NULL END";
      }
      return columnToCalc;
    };

    if(getType(column.measureChannel) == MeasureType::INTENSITY) {
      std::string tablePrefix = " tj" + std::to_string(column.crossChannelStacksC) + ".";
      std::string meas_suffix;
      if(isOuter) {
        tablePrefix = " ";
        meas_suffix = "_" + std::to_string(column.crossChannelStacksC);
      }

      channels += getStatsString(stats) + "(" + injectCase(tablePrefix + createName() + meas_suffix) + ") as " +
                  getMeasurement(column.measureChannel, true) + "_" + getStatsString(column.stats) + "_" +
                  std::to_string(column.crossChannelStacksC) + ",\n";

    } else {
      std::string tablePrefix = " t1.";
      if(isOuter || column.measureChannel == enums::Measurement::COUNT) {
        tablePrefix = " ";
      }
      channels += getStatsString(stats) + "(" + injectCase(tablePrefix + createName()) + ") as " + getMeasurement(column.measureChannel, true) + "_" +
                  getStatsString(column.stats) + ",\n";
    }
  }

  return channels;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::string PreparedStatement::createStatsQueryJoins() const
{
  std::set<uint32_t> joindStacks;
  std::string joins;
  for(const auto &[_, column] : columns) {
    if(getType(column.measureChannel) == MeasureType::INTENSITY) {
      if(!joindStacks.contains(column.crossChannelStacksC)) {
        std::string tableName = "tj" + std::to_string(column.crossChannelStacksC);
        joins += "LEFT JOIN object_measurements " + tableName + " ON\n   t1.object_id = " + tableName + ".object_id AND t1.image_id = " + tableName +
                 ".image_id  AND meas_stack_c = " + std::to_string(column.crossChannelStacksC) +
                 " AND meas_stack_z = " + std::to_string(column.zStack) + " AND meas_stack_t = " + std::to_string(column.tStack) + "\n";

        joindStacks.emplace(column.crossChannelStacksC);
      }
    }
  }

  return joins;
}

std::string PreparedStatement::getStatsString(enums::Stats stats)
{
  std::string statsStr;
  switch(stats) {
    case enums::Stats::AVG:
      statsStr = "AVG";
      break;
    case enums::Stats::MEDIAN:
      statsStr = "MEDIAN";
      break;
    case enums::Stats::SUM:
      statsStr = "SUM";
      break;
    case enums::Stats::MIN:
      statsStr = "MIN";
      break;
    case enums::Stats::MAX:
      statsStr = "MAX";
      break;
    case enums::Stats::STDDEV:
      statsStr = "STDDEV";
      break;
    case enums::Stats::CNT:
      statsStr = "COUNT";
      break;
    case enums::Stats::OFF:
      statsStr = "ANY_VALUE";
      break;
  };
  return statsStr;
}

std::string PreparedStatement::getMeasurement(enums::Measurement measure, bool textual)
{
  switch(measure) {
    case enums::Measurement::COUNT:
      if(textual) {
        return "counted";
      }
      return "1";
    case enums::Measurement::CENTER_OF_MASS_X:
      return "meas_center_x";
    case enums::Measurement::CENTER_OF_MASS_Y:
      return "meas_center_y";
    case enums::Measurement::CONFIDENCE:
      return "meas_confidence";
    case enums::Measurement::AREA_SIZE:
      return "meas_area_size";
    case enums::Measurement::PERIMETER:
      return "meas_perimeter";
    case enums::Measurement::CIRCULARITY:
      return "meas_circularity";
    case enums::Measurement::INTENSITY_SUM:
      return "meas_intensity_sum";
    case enums::Measurement::INTENSITY_AVG:
      return "meas_intensity_avg";
    case enums::Measurement::INTENSITY_MIN:
      return "meas_intensity_min";
    case enums::Measurement::INTENSITY_MAX:
      return "meas_intensity_max";
    case enums::Measurement::BOUNDING_BOX_WIDTH:
      return "meas_box_width";
    case enums::Measurement::BOUNDING_BOX_HEIGHT:
      return "meas_box_height";
    case enums::Measurement::ORIGIN_OBJECT_ID:
      return "meas_origin_object_id";
      break;
  }
  if(textual) {
    return "none";
  }
  return "1";
}

}    // namespace joda::db
