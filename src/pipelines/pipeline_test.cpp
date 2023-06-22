#include "image_reader/jpg/image_loader_jpg.hpp"
#include "settings/analze_settings_parser.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include "pipeline_factory.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test", "[pipeline_test]")
{
  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromFile("test/nuclues_test.json");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test");

  while(true) {
    sleep(2);
  }
}
