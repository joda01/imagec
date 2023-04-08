#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include "analze_settings.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("analze_settings", "[analze_settings]")
{
  AnalyzeSettings settings;

  settings.loadConfig("src/settings/config_test.json");
  CHECK(settings.min_coloc_factor == 15.5);
  CHECK(settings.pixel_in_micrometer == 0.001F);

  /*REQUIRE( 3 == add(1,2));
  REQUIRE( 3 == add(1,2));*/
}
