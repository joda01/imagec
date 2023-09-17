#include "../settings/channel_settings.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include "analze_settings_parser.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("analze_settings", "[analze_settings]")
{
  joda::settings::json::AnalyzeSettings settings;

  settings.loadConfigFromFile("src/settings/config_test.json");
  CHECK(settings.getOptions().getPixelInMicrometer() == 0.001F);

  auto evChannels = settings.getChannels(joda::settings::json::ChannelInfo::Type::SPOT);
  CHECK(evChannels.size() == 2);
  CHECK(evChannels[0].getChannelInfo().getLabel() == "CY5");
  CHECK(evChannels[0].getDetectionSettings().getThersholdSettings().getThreshold() ==
        joda::settings::json::ThresholdSettings::Threshold::LI);
  CHECK(evChannels[0].getDetectionSettings().getThersholdSettings().getThreshold() ==
        joda::settings::json::ThresholdSettings::Threshold::LI);

  /*REQUIRE( 3 == add(1,2));
  REQUIRE( 3 == add(1,2));*/
}
