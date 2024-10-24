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
    if(!mClustersAndClasses.contains(colKey.clusterClass)) {
      mClustersAndClasses.emplace(colKey.clusterClass, PreparedStatement{colKey.names});
    }
    mClustersAndClasses.at(colKey.clusterClass).addColumn(colKey);
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
std::string PreparedStatement::createStatsQuery(bool isDistinct, std::optional<enums::Stats> overrideStats) const
{
  std::string distinct = isDistinct ? "DISTINCT" : "";
  std::string channels;
  for(const auto &[_, column] : columns) {
    auto createName = [&column = column, &isDistinct]() -> std::string {
      if(isDistinct) {
        return getMeasurement(column.measureChannel, false);
      } else {
        return getMeasurement(column.measureChannel, true) + "_" + getStatsString(column.stats);
      }
    };

    auto stats = overrideStats.has_value() ? overrideStats.value() : column.stats;

    if(getType(column.measureChannel) == MeasureType::INTENSITY) {
      if(isDistinct) {
        channels += getStatsString(stats) + "(" + distinct + " CASE WHEN t2.meas_stack_c = " + std::to_string(column.crossChannelStacksC) + " THEN " +
                    createName() + " END) AS " + getMeasurement(column.measureChannel, true) + "_" + getStatsString(column.stats) + "_" +
                    std::to_string(column.crossChannelStacksC) + ",\n";
      } else {
        channels += getStatsString(stats) + "(" + createName() + "_" + std::to_string(column.crossChannelStacksC) + ") AS " +
                    getMeasurement(column.measureChannel, true) + "_" + getStatsString(column.stats) + "_" +
                    std::to_string(column.crossChannelStacksC) + ",\n";
      }

    } else {
      channels += getStatsString(stats) + "(" + distinct + " " + createName() + ") as " + getMeasurement(column.measureChannel, true) + "_" +
                  getStatsString(column.stats) + ",\n";
    }
  }

  return channels;
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
