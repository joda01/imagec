

#pragma once

#include <cstdint>
#include <string>
#include "backend/settings/channel/channel_index.hpp"
#include "backend/settings/channel/channel_reporting_settings.hpp"

namespace joda::settings {
class AnalyzeSettings;
}    // namespace joda::settings

namespace joda::pipeline::reporting {

static inline const uint32_t MEASURE_CHANNEL_MASK       = 0x000000FF;
static inline const uint32_t MEASURE_CHANNEL_INDEX_MASK = 0x0000FF00;

static inline joda::settings::ChannelReportingSettings::MeasureChannels
getMeasureChannel(uint32_t measureChannelUnmasked)
{
  return static_cast<joda::settings::ChannelReportingSettings::MeasureChannels>(measureChannelUnmasked &
                                                                                MEASURE_CHANNEL_MASK);
}

static inline joda::settings::ChannelIndex getChannelIndexFromMeasureChannel(uint32_t measureChannelUnmasked)
{
  return static_cast<joda::settings::ChannelIndex>((measureChannelUnmasked & MEASURE_CHANNEL_INDEX_MASK) >> 8);
}

static inline uint32_t
getMaskedMeasurementChannel(joda::settings::ChannelReportingSettings::MeasureChannels measureChannel,
                            joda::settings::ChannelIndex channelIndex)
{
  return ((uint32_t) channelIndex << 8) | (static_cast<uint32_t>(measureChannel));
}

static inline std::string measurementChannelsToString(joda::settings::ChannelReportingSettings::MeasureChannels ch)
{
  switch(ch) {
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
    case joda::settings::ChannelReportingSettings::MeasureChannels::INTERSECTION_CROSS_CHANNEL:
      return "Counted in";
  }
}

extern std::string measurementChannelsToString(uint32_t ch, const joda::settings::AnalyzeSettings &analyzeSettings);

}    // namespace joda::pipeline::reporting
