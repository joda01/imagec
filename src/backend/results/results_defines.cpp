///
/// \file      results_defines.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#include "results_defines.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/settings.hpp"

namespace joda::results {

[[nodiscard]] joda::settings::ChannelReportingSettings::MeasureChannels MeasureChannelKey::getMeasureChannel() const
{
  return static_cast<joda::settings::ChannelReportingSettings::MeasureChannels>((value & MEASURE_CHANNEL_MASK) >> 16);
}

[[nodiscard]] joda::settings::ChannelReportingSettings::MeasureChannelStat MeasureChannelKey::getMeasureStats() const
{
  return static_cast<joda::settings::ChannelReportingSettings::MeasureChannelStat>(
      (value & MEASURE_CHANNEL_STATS_MASK) >> 8);
}

[[nodiscard]] joda::settings::ChannelIndex MeasureChannelKey::getChannelIndex() const
{
  return static_cast<joda::settings::ChannelIndex>((value & MEASURE_CHANNEL_INDEX_MASK) & MEASURE_CHANNEL_INDEX_MASK);
}

[[nodiscard]] std::string MeasureChannelKey::measurementStatsToString() const
{
  switch(getMeasureStats()) {
    case settings::ChannelReportingSettings::MeasureChannelStat::AVG:
      return "avg";
    case settings::ChannelReportingSettings::MeasureChannelStat::SUM:
      return "sum";
    case settings::ChannelReportingSettings::MeasureChannelStat::MIN:
      return "min";
    case settings::ChannelReportingSettings::MeasureChannelStat::MAX:
      return "max";
    case settings::ChannelReportingSettings::MeasureChannelStat::CNT:
      return "cnt";
    case settings::ChannelReportingSettings::MeasureChannelStat::STD_DEV:
      return "std devi.";
  }
  return "";
}

[[nodiscard]] std::string MeasureChannelKey::measurementChannelsToString() const
{
  switch(getMeasureChannel()) {
    case joda::settings::ChannelReportingSettings::MeasureChannels::CONFIDENCE:
      return "Confidence";
    case joda::settings::ChannelReportingSettings::MeasureChannels::AREA_SIZE:
      return "Area size";
    case joda::settings::ChannelReportingSettings::MeasureChannels::PERIMETER:
      return "Perimeter";
    case joda::settings::ChannelReportingSettings::MeasureChannels::CIRCULARITY:
      return "Circularity";
    case joda::settings::ChannelReportingSettings::MeasureChannels::VALIDITY:
      return "Validity";
    case joda::settings::ChannelReportingSettings::MeasureChannels::INVALIDITY:
      return "Invalidity";
    case joda::settings::ChannelReportingSettings::MeasureChannels::CENTER_OF_MASS_X:
      return "X";
    case joda::settings::ChannelReportingSettings::MeasureChannels::CENTER_OF_MASS_Y:
      return "Y";
    case joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_AVG:
      return "Intensity Avg.";
    case joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_MIN:
      return "Intensity Min.";
    case joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_MAX:
      return "Intensity Max.";
    case joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL:
      return "Intensity Avg. in";
    case joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL:
      return "Intensity Min. in";
    case joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL:
      return "Intensity Max. in";
    case joda::settings::ChannelReportingSettings::MeasureChannels::COUNT_CROSS_CHANNEL:
      return "Counted in";
  }
  return "UNKNOWN MEASURE CHANNEL";
}

std::string MeasureChannelKey::measurementChannelsToString(const joda::settings::AnalyzeSettings &settings) const
{
  return measurementChannelsToString() + " (" + measurementStatsToString() + ") " +
         joda::settings::Settings::getChannelNameOfChannelIndex(settings, getChannelIndex());
}

}    // namespace joda::results
