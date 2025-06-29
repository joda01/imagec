///
/// \file      results_settings.cpp
/// \author    Joachim Danmayr
/// \date      2025-06-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "results_settings.hpp"

namespace joda::settings {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::string ResultsSettings::ColumnKey::createHeader() const
{
  std::map<uint32_t, std::string> columnHeaders;
  std::string stacks = "{Z" + std::to_string(zStack) + "}";

  auto createStatsHeader = [](enums::Stats stats) -> std::string {
    if(stats != enums::Stats::OFF) {
      return "[" + enums::toString(stats) + "]";
    }
    return "";
  };

  if(getType(measureChannel) == MeasureType::INTENSITY) {
    return names.className + "-" + toString(measureChannel) + createStatsHeader(stats) + " " + "(C" + std::to_string(crossChannelStacksC) + ")" +
           stacks;
  }
  if(getType(measureChannel) == MeasureType::INTERSECTION) {
    return "Nr. of " + names.intersectingName + " in " + names.className + createStatsHeader(stats) + stacks;
  }
  if(getType(measureChannel) == MeasureType::ID) {
    return names.className + "-" + toString(measureChannel) + "\n" + stacks;
  }
  if(getType(measureChannel) == MeasureType::DISTANCE) {
    return names.className + " to " + names.intersectingName + "-" + toString(measureChannel) + createStatsHeader(stats) + stacks;
  }
  if(getType(measureChannel) == MeasureType::DISTANCE_ID) {
    return names.className + " to " + names.intersectingName + "-" + toString(measureChannel) + stacks;
  }

  return names.className + "-" + toString(measureChannel) + createStatsHeader(stats) + stacks;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::string ResultsSettings::ColumnKey::createHtmlHeader(HeaderStyle style) const
{
  std::map<uint32_t, std::string> columnHeaders;
  std::string stacks = "{Z" + std::to_string(zStack) + "}";

  auto createStatsHeader = [](enums::Stats stats) -> std::string {
    if(stats != enums::Stats::OFF) {
      return "[" + enums::toString(stats) + "]";
    }
    return "";
  };

  std::string className = "<b>" + names.className + "</b><br>";
  if(style != HeaderStyle::FULL) {
    className = "";
  }

  if(getType(measureChannel) == MeasureType::INTENSITY) {
    return className + toString(measureChannel) + " " + createStatsHeader(stats) + " " + "(C" + std::to_string(crossChannelStacksC) + ")" + stacks;
  }
  if(getType(measureChannel) == MeasureType::INTERSECTION) {
    if(style == HeaderStyle::FULL) {
      return "<b>" + names.intersectingName + "</b><br>in<br><b>" + names.className + "</b><br>" + createStatsHeader(stats) + "<br>" + stacks;
    } else if(style == HeaderStyle::ONLY_STATS_IN_INTERSECTING) {
      return "Counted in " + names.className + "</b> " + createStatsHeader(stats) + "<br>" + stacks;
    } else if(style == HeaderStyle::ONLY_STATS_CONTAINS_INTERSECTING) {
      return "Contains number of " + names.intersectingName + "</b> " + createStatsHeader(stats) + "<br>" + stacks;
    }
  }
  if(getType(measureChannel) == MeasureType::ID) {
    return className + toString(measureChannel) + "\n" + stacks;
  }
  if(getType(measureChannel) == MeasureType::DISTANCE) {
    return className + " to " + names.intersectingName + "-" + toString(measureChannel) + createStatsHeader(stats) + stacks;
  }
  if(getType(measureChannel) == MeasureType::DISTANCE_ID) {
    return className + " to " + names.intersectingName + "-" + toString(measureChannel) + stacks;
  }

  return className + toString(measureChannel) + " " + createStatsHeader(stats) + "<br>" + stacks;
}

}    // namespace joda::settings
