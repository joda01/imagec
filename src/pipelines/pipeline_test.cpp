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
  settings.loadConfigFromFile("test_areosold_Evs/config.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test_areosold_Evs");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test_areosold_Evs", &imageFileContainer);

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:spots", "[pipeline_test_spots]")
{
  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromFile("test_areosold_Evs/config.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test_areosold_Evs");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test_areosold_Evs", &imageFileContainer);

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:cells", "[pipeline_test_cells]")
{
  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromFile("test_cell/config.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test_cell");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test_cell", &imageFileContainer);

  while(true) {
    sleep(2);
  }
}
