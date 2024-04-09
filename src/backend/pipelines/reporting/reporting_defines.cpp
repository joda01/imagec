
#include "reporting_defines.hpp"
#include "backend/settings/analze_settings_parser.hpp"

namespace joda::pipeline::reporting {

std::string measurementChannelsToString(uint32_t ch, const joda::settings::json::AnalyzeSettings &analyzeSettings)
{
  return measurementChannelsToString(getMeasureChannel(ch)) + " " +
         analyzeSettings.getChannelNameOfIndex(getChannelIndexFromMeasureChannel(ch));
}

}    // namespace joda::pipeline::reporting
