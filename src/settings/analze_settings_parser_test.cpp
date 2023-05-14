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
  CHECK(settings.getMinColocFactor() == 15.5);
  CHECK(settings.getPixelInMicrometer() == 0.001F);
  CHECK(settings.getPipeline() == joda::settings::json::AnalyzeSettings::Pipeline::NUCLEUS_COUNT);

  auto evChannels = settings.getChannels(joda::settings::json::ChannelSettings::Type::EV);
  CHECK(evChannels.size() == 2);
  CHECK(evChannels[0].getLabel() == "CY5");

  /*REQUIRE( 3 == add(1,2));
  REQUIRE( 3 == add(1,2));*/
}
