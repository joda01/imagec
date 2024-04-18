
#include "reporting_defines.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/settings.hpp"

namespace joda::pipeline::reporting {

std::string measurementChannelsToString(uint32_t ch, const joda::settings::AnalyzeSettings &settings)
{
  return measurementChannelsToString(getMeasureChannel(ch)) + measurementStatsToString(getMeasureStats(ch)) + " " +
         joda::settings::Settings::getChannelNameOfChannelIndex(settings, getChannelIndexFromMeasureChannel(ch));
}

}    // namespace joda::pipeline::reporting
