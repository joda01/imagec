///
/// \file      filter.cpp
/// \author    Joachim Danmayr
/// \date      2024-10-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "filter.hpp"
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
ResultingTable::ResultingTable(const settings::ResultsSettings *filter) : mFilter(filter)
{
  std::map<uint32_t, settings::ResultsSettings::ColumnKey> tableHeaders;
  for(const auto &[colIdx, colKey] : filter->getColumns()) {
    QueryKey qKey;
    if(settings::ResultsSettings::getType(colKey.measureChannel) == settings::ResultsSettings::MeasureType::DISTANCE ||
       settings::ResultsSettings::getType(colKey.measureChannel) == settings::ResultsSettings::MeasureType::DISTANCE_ID) {
      // For distance measurement we have to create a separate statement for each distance from - distance to class combination
      // Distance from is the colKey.classId, distance to is the colKey.intersectingChannel
      qKey = {colKey.classId, colKey.zStack, filter->getFilter().tStack, colKey.intersectingChannel};
    } else {
      qKey = {colKey.classId, colKey.zStack, filter->getFilter().tStack, joda::enums::ClassId::NONE};
    }
    if(!mClassesAndClasses.contains(qKey)) {
      mClassesAndClasses.emplace(qKey, PreparedStatement{colKey.names, filter});
    }
    mClassesAndClasses.at(qKey).addColumn(colKey);
    mTableMapping.emplace(colKey, colIdx);
    tableHeaders.emplace(colIdx.colIdx, colKey);
  }

  mResultingTable.setColHeader(tableHeaders);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::tuple<std::string, std::string> PreparedStatement::createIntersectionQuery() const
{
  std::string retValSum;
  std::string retValCnt;

  for(const auto &[_, column] : columns) {
    if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::INTERSECTION) {
      std::string chStr = std::to_string(static_cast<int32_t>(column.intersectingChannel));
      retValSum += "SUM(CASE WHEN ad.class_id = " + chStr + " THEN 1 ELSE 0 END) AS recursive_child_count_" + chStr + ",\n";
    }
  }

  auto removeSuffix = [](std::string &str) {
    std::string target      = ",\n";
    std::string replacement = "\n";
    size_t pos              = str.rfind(target);    // Find the last occurrence of ",\n"
    if(pos != std::string::npos && pos == str.length() - target.length()) {
      str.replace(pos, target.length(), replacement);    // Replace ",\n" with "\n"
    }
  };

  removeSuffix(retValSum);
  removeSuffix(retValCnt);

  return {retValSum, retValCnt};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::string PreparedStatement::createStatsQuery(bool isOuter, bool excludeInvalid, std::string offValue,
                                                std::optional<enums::Stats> overrideStats) const
{
  std::string channels;
  for(const auto &[_, column] : columns) {
    auto createName = [&column = column, &isOuter, &offValue](enums::Stats stats) -> std::string {
      if(!isOuter) {
        return getMeasurement(column.measureChannel, false);
      } else {
        return getMeasurement(column.measureChannel, true) + "_" + getStatsString(stats, offValue);
      }
    };

    auto stats = overrideStats.has_value() ? overrideStats.value() : column.stats;

    auto injectCase = [&excludeInvalid](const std::string &columnToCalc) {
      if(excludeInvalid) {
        return "CASE WHEN validity = 0 THEN " + columnToCalc + " ELSE NULL END";
      }
      return columnToCalc;
    };

    if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::INTENSITY) {
      std::string tablePrefix = " tj" + std::to_string(column.crossChannelStacksC) + ".";
      std::string meas_suffix;
      if(isOuter) {
        tablePrefix = " ";
        meas_suffix = "_" + std::to_string(column.crossChannelStacksC);
      }

      channels += getStatsString(stats, offValue) + "(" + injectCase(tablePrefix + createName(column.stats) + meas_suffix) + ") as " +
                  getMeasurement(column.measureChannel, true) + "_" + getStatsString(column.stats, offValue) + "_" +
                  std::to_string(column.crossChannelStacksC) + ",\n";

    } else if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::INTERSECTION) {
      std::string colName;
      std::string chStr = std::to_string(static_cast<int32_t>(column.intersectingChannel));
      if(offValue == "ANY_VALUE") {
        offValue = "AVG";    // For intersecting the OFF value should be AVG. This is leghacy and can be removed
      }

      if(!isOuter) {
        colName = "recursive_child_count_" + chStr;
      } else {
        colName = " recursive_child_count_" + chStr + "_" + getStatsString(column.stats, offValue);
      }

      std::string tablePrefix = " ti.";
      if(isOuter || column.measureChannel == enums::Measurement::COUNT) {
        tablePrefix = " ";
      }
      channels += getStatsString(stats, offValue) + "(" + injectCase(tablePrefix + colName) + ") as " + "recursive_child_count_" + chStr + "_" +
                  getStatsString(column.stats, offValue) + ",\n";

    } else if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::ID) {
      std::string tablePrefix = " t1.";
      if(isOuter || column.measureChannel == enums::Measurement::COUNT) {
        tablePrefix = " ";
      }
      // We show the smallest object ID if we are in an overview mode
      channels += getStatsString(enums::Stats::OFF, offValue) + "(" + injectCase(tablePrefix + createName(enums::Stats::OFF)) + ") as " +
                  getMeasurement(column.measureChannel, true) + "_" + getStatsString(enums::Stats::OFF, offValue) + ",\n";
    } else if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::DISTANCE_ID) {
      std::string tablePrefix = " td.";
      if(isOuter || column.measureChannel == enums::Measurement::COUNT) {
        tablePrefix = " ";
      }
      // We show the smallest object ID if we are in an overview mode
      channels += getStatsString(enums::Stats::OFF, offValue) + "(" + injectCase(tablePrefix + createName(enums::Stats::OFF)) + ") as " +
                  getMeasurement(column.measureChannel, true) + "_" + getStatsString(enums::Stats::OFF, offValue) + ",\n";
    } else if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::DISTANCE) {
      std::string tablePrefix = " td.";
      if(isOuter || column.measureChannel == enums::Measurement::COUNT) {
        tablePrefix = " ";
      }
      channels += getStatsString(stats, offValue) + "(" + injectCase(tablePrefix + createName(column.stats)) + ") as " +
                  getMeasurement(column.measureChannel, true) + "_" + getStatsString(column.stats, offValue) + ",\n";
    } else {
      std::string tablePrefix = " t1.";
      if(isOuter || column.measureChannel == enums::Measurement::COUNT) {
        tablePrefix = " ";
      }
      channels += getStatsString(stats, offValue) + "(" + injectCase(tablePrefix + createName(column.stats)) + ") as " +
                  getMeasurement(column.measureChannel, true) + "_" + getStatsString(column.stats, offValue) + ",\n";
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
std::string PreparedStatement::createStatsQueryJoins(bool isImage, JoinResults *results) const
{
  bool joinedDistance = false;
  std::set<uint32_t> joindStacks;
  std::set<enums::ClassId> joindDistanceStacks;
  std::string joins;
  bool intersectionJoin = false;
  for(const auto &[_, column] : columns) {
    if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::INTENSITY) {
      if(!joindStacks.contains(column.crossChannelStacksC)) {
        std::string tableName = "tj" + std::to_string(column.crossChannelStacksC);
        joins += "LEFT JOIN object_measurements " + tableName + " ON\n   t1.object_id = " + tableName + ".object_id AND t1.image_id = " + tableName +
                 ".image_id  AND " + tableName + ".meas_stack_c = " + std::to_string(column.crossChannelStacksC) + " AND " + tableName +
                 " .meas_stack_z = " + std::to_string(column.zStack) + " AND " + tableName +
                 ".meas_stack_t = " + std::to_string(mFilter->getFilter().tStack) + "\n";

        joindStacks.emplace(column.crossChannelStacksC);
        if(nullptr != results) {
          results->containsIntensity = true;
        }
      }
    }
    if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::DISTANCE ||
       settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::DISTANCE_ID) {
      if(!joindDistanceStacks.contains(column.intersectingChannel)) {
        std::string tableName = "td";
        std::string chStr     = std::to_string(static_cast<int32_t>(column.intersectingChannel));
        joins += "LEFT JOIN distance_measurements " + tableName + " ON\n   t1.object_id = " + tableName +
                 ".object_id AND t1.image_id = " + tableName + ".image_id  AND " + tableName + ".meas_class_id = " + chStr + " AND " + tableName +
                 " .meas_stack_z = " + std::to_string(column.zStack) + " AND " + tableName +
                 ".meas_stack_t = " + std::to_string(mFilter->getFilter().tStack) + "\n";

        joindDistanceStacks.emplace(column.intersectingChannel);
        if(nullptr != results) {
          results->containsDistance = true;
        }
      }
    }

    if(settings::ResultsSettings::getType(column.measureChannel) == settings::ResultsSettings::MeasureType::INTERSECTION) {
      if(!intersectionJoin) {
        intersectionJoin = true;
        joins += "LEFT JOIN TblIntersecting ti on ti.image_id = t1.image_id";
        if(isImage) {
          joins += " AND ti.object_id = t1.object_id\n";
        } else {
          joins += "\n";
        }
        if(nullptr != results) {
          results->containsIntersection = true;
        }
      }
    }
  }

  return joins;
}

std::string PreparedStatement::getStatsString(enums::Stats stats, const std::string &offValue)
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
      statsStr = offValue;
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
    case enums::Measurement::CENTEROID_X:
      return "meas_center_x";
    case enums::Measurement::CENTEROID_Y:
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
    case enums::Measurement::OBJECT_ID:
      return "object_id";
    case enums::Measurement::ORIGIN_OBJECT_ID:
      return "meas_origin_object_id";
    case enums::Measurement::PARENT_OBJECT_ID:
      return "meas_parent_object_id";
    case enums::Measurement::TRACKING_ID:
      return "meas_tracking_id";
    case enums::Measurement::INTERSECTING:
      return "recursive_child_count_";
    case enums::Measurement::DISTANCE_CENTER_TO_CENTER:
      return "meas_distance_center_to_center";
    case enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MIN:
      return "meas_distance_center_to_surface_min";
    case enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MAX:
      return "meas_distance_center_to_surface_max";
    case enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN:
      return "meas_distance_surface_to_surface_min";
    case enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX:
      return "meas_distance_surface_to_surface_max";
    case enums::Measurement::DISTANCE_FROM_OBJECT_ID:
      return "object_id";
    case enums::Measurement::DISTANCE_TO_OBJECT_ID:
      return "meas_object_id";
  }
  if(textual) {
    return "none";
  }
  return "1";
}

}    // namespace joda::db
