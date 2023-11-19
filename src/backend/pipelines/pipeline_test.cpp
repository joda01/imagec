#include "../image_reader/jpg/image_loader_jpg.hpp"
#include "../settings/analze_settings_parser.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "controller/controller.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include "pipeline_factory.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test", "[pipeline_test]")
{
  for(int n = 0; n < 4; n++) {
    joda::settings::json::AnalyzeSettings settings;
    settings.loadConfigFromFile("test_areosold_Evs/config.json");
    joda::helper::ImageFileContainer imageFileContainer;
    imageFileContainer.setWorkingDirectory("test_areosold_Evs");
    joda::pipeline::PipelineFactory::startNewJob(settings, "test_areosold_Evs", &imageFileContainer);

    joda::pipeline::Pipeline::State state = joda::pipeline::Pipeline::State::STOPPED;
    while(state != joda::pipeline::Pipeline::State::FINISHED) {
      auto [a, b, c] = joda::pipeline::PipelineFactory::getState("");
      state          = b;
      sleep(1);
    }
    std::cout << "Run finished" << std::endl;
    sleep(10);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:voronoi", "[pipeline_test_voronoi]")
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
  settings.loadConfigFromFile("test_spot/config.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test_spot");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test_spot", &imageFileContainer);

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
  settings.loadConfigFromFile("test/test_cell/config.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_cell");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_cell", &imageFileContainer);

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:cell_area", "[pipeline_test_cell_area]")
{
  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromFile("test/test_cell/config_cell.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_cell");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_cell", &imageFileContainer);

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:nucleus", "[pipeline_test_nucleus]")
{
  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromFile("test_nucleus/config.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test_nucleus");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test_nucleus", &imageFileContainer);

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:spots_real", "[pipeline_test_spots_real]")
{
  joda::ctrl::Controller controller;
  controller.setWorkingDirectory("test/test_spot/evanalyzer_comp");
  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromFile("test/test_spot/evanalyzer_comp/mysettings.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_spot/evanalyzer_comp");
  sleep(1);
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_spot/evanalyzer_comp", &imageFileContainer,
                                               controller.calcOptimalThreadNumber(settings, 0));

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:histo", "[pipeline_test_histo]")
{
  BioformatsLoader::init();

  joda::ctrl::Controller controller;
  controller.setWorkingDirectory("test/test_histo");
  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromFile("test/test_histo/settings.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_histo");
  sleep(1);
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_histo", &imageFileContainer,
                                               controller.calcOptimalThreadNumber(settings, 0));

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:spots:tetraspeck", "[pipeline_test_spots_tetraspeck]")
{
  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromFile("test/test_spot/config_tetra.json");
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_spot");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_spot", &imageFileContainer);

  while(true) {
    sleep(2);
  }
}
