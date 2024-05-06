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

[[nodiscard]] joda::results::ReportingSettings::MeasureChannels MeasureChannelKey::getMeasureChannel() const
{
  return static_cast<joda::results::ReportingSettings::MeasureChannels>((value & MEASURE_CHANNEL_MASK) >> 16);
}

[[nodiscard]] joda::results::ReportingSettings::MeasureChannelStat MeasureChannelKey::getMeasureStats01() const
{
  return static_cast<joda::results::ReportingSettings::MeasureChannelStat>((value & MEASURE_CHANNEL_STATS_01_MASK) >>
                                                                           8);
}

[[nodiscard]] joda::results::ReportingSettings::MeasureChannelStat MeasureChannelKey::getMeasureStats02() const
{
  return static_cast<joda::results::ReportingSettings::MeasureChannelStat>((value & MEASURE_CHANNEL_STATS_02_MASK) >>
                                                                           12);
}

[[nodiscard]] joda::settings::ChannelIndex MeasureChannelKey::getChannelIndex() const
{
  return static_cast<joda::settings::ChannelIndex>((value & MEASURE_CHANNEL_INDEX_MASK) & MEASURE_CHANNEL_INDEX_MASK);
}

[[nodiscard]] ReportingSettings::MeasureChannelsCombi MeasureChannelKey::getMeasreChannelAndStatsCombo() const
{
  return static_cast<ReportingSettings::MeasureChannelsCombi>((value & MEASURE_CHANNEL_STATS_AND_MASK));
}

[[nodiscard]] std::string MeasureChannelKey::measurementStatsToString() const
{
  switch(getMeasureStats01()) {
    case ReportingSettings::MeasureChannelStat::AVG:
      return "avg";
    case ReportingSettings::MeasureChannelStat::SUM:
      return "sum";
    case ReportingSettings::MeasureChannelStat::MIN:
      return "min";
    case ReportingSettings::MeasureChannelStat::MAX:
      return "max";
    case ReportingSettings::MeasureChannelStat::CNT:
      return "cnt";
    case ReportingSettings::MeasureChannelStat::STD_DEV:
      return "std devi.";
  }
  return "";
}

[[nodiscard]] std::string MeasureChannelKey::measurementChannelsToString() const
{
  switch(getMeasureChannel()) {
    case joda::results::ReportingSettings::MeasureChannels::CONFIDENCE:
      return "Confidence";
    case joda::results::ReportingSettings::MeasureChannels::AREA_SIZE:
      return "Area size";
    case joda::results::ReportingSettings::MeasureChannels::PERIMETER:
      return "Perimeter";
    case joda::results::ReportingSettings::MeasureChannels::CIRCULARITY:
      return "Circularity";
    case joda::results::ReportingSettings::MeasureChannels::VALIDITY:
      return "Validity";
    case joda::results::ReportingSettings::MeasureChannels::INVALIDITY:
      return "Invalidity";
    case joda::results::ReportingSettings::MeasureChannels::CENTER_OF_MASS_X:
      return "X";
    case joda::results::ReportingSettings::MeasureChannels::CENTER_OF_MASS_Y:
      return "Y";
    case joda::results::ReportingSettings::MeasureChannels::INTENSITY_AVG:
      return "Intensity Avg.";
    case joda::results::ReportingSettings::MeasureChannels::INTENSITY_MIN:
      return "Intensity Min.";
    case joda::results::ReportingSettings::MeasureChannels::INTENSITY_MAX:
      return "Intensity Max.";
    case joda::results::ReportingSettings::MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL:
      return "Intensity Avg. in";
    case joda::results::ReportingSettings::MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL:
      return "Intensity Min. in";
    case joda::results::ReportingSettings::MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL:
      return "Intensity Max. in";
    case joda::results::ReportingSettings::MeasureChannels::COUNT_CROSS_CHANNEL:
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
