

#pragma once

#include <string>
#include <cstdint>

namespace joda::settings::json {
class AnalyzeSettings;
}    // namespace joda::settings::json

namespace joda::pipeline::reporting {

static inline const uint32_t MEASURE_CHANNEL_MASK       = 0x000000FF;
static inline const uint32_t MEASURE_CHANNEL_INDEX_MASK = 0x0000FF00;

/////////////////////////////////////////////////////
enum class MeasurementChannels : int
{
  CONFIDENCE                  = 0x0001,
  AREA_SIZE                   = 0x0002,
  PERIMETER                   = 0x0003,
  CIRCULARITY                 = 0x0004,
  VALIDITY                    = 0x0005,
  INVALIDITY                  = 0x0006,
  CENTER_OF_MASS_X            = 0x0007,
  CENTER_OF_MASS_Y            = 0x0008,
  INTENSITY_AVG               = 0x0009,    // Start of the dynamic section
  INTENSITY_MIN               = 0x000A,
  INTENSITY_MAX               = 0x000B,
  INTENSITY_AVG_CROSS_CHANNEL = 0x000C,    // Start of the dynamic section
  INTENSITY_MIN_CROSS_CHANNEL = 0x000D,
  INTENSITY_MAX_CROSS_CHANNEL = 0x000E,
  INTERSECTION_CROSS_CHANNEL  = 0x000F
};

static inline MeasurementChannels getMeasureChannel(uint32_t measureChannelUnmasked)
{
  return static_cast<MeasurementChannels>(measureChannelUnmasked & MEASURE_CHANNEL_MASK);
}

static inline uint32_t getChannelIndexFromMeasureChannel(uint32_t measureChannelUnmasked)
{
  return (measureChannelUnmasked & MEASURE_CHANNEL_INDEX_MASK) >> 8;
}

static inline uint32_t getMaskedMeasurementChannel(MeasurementChannels measureChannel, uint32_t channelIndex)
{
  return (channelIndex << 8) | (static_cast<uint32_t>(measureChannel));
}

static inline std::string measurementChannelsToString(MeasurementChannels ch)
{
  switch(ch) {
    case MeasurementChannels::CONFIDENCE:
      return "Confidence";
    case MeasurementChannels::AREA_SIZE:
      return "Area size";
    case MeasurementChannels::PERIMETER:
      return "Perimeter";
    case MeasurementChannels::CIRCULARITY:
      return "Circularity";
    case MeasurementChannels::VALIDITY:
      return "Validity";
    case MeasurementChannels::INVALIDITY:
      return "Invalidity";
    case MeasurementChannels::CENTER_OF_MASS_X:
      return "X";
    case MeasurementChannels::CENTER_OF_MASS_Y:
      return "Y";
    case MeasurementChannels::INTENSITY_AVG:
      return "Intensity Avg.";
    case MeasurementChannels::INTENSITY_MIN:
      return "Intensity Min.";
    case MeasurementChannels::INTENSITY_MAX:
      return "Intensity Max.";
    case MeasurementChannels::INTENSITY_AVG_CROSS_CHANNEL:
      return "Intensity Avg. in";
    case MeasurementChannels::INTENSITY_MIN_CROSS_CHANNEL:
      return "Intensity Min. in";
    case MeasurementChannels::INTENSITY_MAX_CROSS_CHANNEL:
      return "Intensity Max. in";
    case MeasurementChannels::INTERSECTION_CROSS_CHANNEL:
      return "Counted in";
  }
}

extern std::string measurementChannelsToString(uint32_t ch,
                                               const joda::settings::json::AnalyzeSettings &analyzeSettings);

}    // namespace joda::pipeline::reporting
