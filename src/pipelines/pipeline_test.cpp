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
  settings.loadConfigFromFile("test_cell/config.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test_cell");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test_cell", &imageFileContainer);

  while(true) {
    sleep(2);
  }
}
