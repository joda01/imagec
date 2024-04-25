#include "../image_reader/jpg/image_loader_jpg.hpp"
#include "../settings/analze_settings.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "controller/controller.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json_fwd.hpp>
#include "pipeline_factory.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test", "[pipeline_test]")
{
  for(int n = 0; n < 4; n++) {
    joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test_areosold_Evs/config.json"});
    joda::helper::ImageFileContainer imageFileContainer;
    imageFileContainer.setWorkingDirectory("test_areosold_Evs");
    joda::pipeline::PipelineFactory::startNewJob(
        settings, "test_areosold_Evs", joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer);

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
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test/test_areosold_Evs/config.json"});
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_areosold_Evs");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_areosold_Evs",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer);

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
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test/test_spot/config.json"});
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_spot");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_spot",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer);

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
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test/test_cell/config.json"});
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_cell");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_cell",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer);

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
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test/test_cell/config_cell.json"});
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_cell");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_cell",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer);

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
  joda::ctrl::Controller controller;
  controller.setWorkingDirectory("test/test_nucleus/");

  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test/test_nucleus/config.json"});
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_nucleus");
  imageFileContainer.waitForFinished();

  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_nucleus",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer,
                                               controller.calcOptimalThreadNumber(settings, 0));

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

  joda::settings::AnalyzeSettings settings =
      nlohmann::json::parse(std::ifstream{"test/test_spot/evanalyzer_comp/mysettings.json"});
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_spot/evanalyzer_comp");
  sleep(1);
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_spot/evanalyzer_comp",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer,
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

  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test/test_histo/settings.json"});
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_histo");
  sleep(1);
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_histo",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer,
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
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test/test_spot/config_tetra.json"});
  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_spot");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_spot",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer);

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:svi_tanja", "[pipeline_test_svi_tanja]")
{
  BioformatsLoader::init();

  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test/test_svi_tanja/config.json"});

  joda::helper::ImageFileContainer imageFileContainer;
  imageFileContainer.setWorkingDirectory("test/test_svi_tanja");
  joda::pipeline::PipelineFactory::startNewJob(settings, "test/test_svi_tanja",
                                               joda::helper::RandomNameGenerator::GetRandomName(), &imageFileContainer);

  while(true) {
    sleep(2);
  }
}

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:heatmap_small", "[pipeline_test_heatmap_small]")
{
  BioformatsLoader::init();
  joda::ctrl::Controller controller;

  joda::settings::AnalyzeSettings settings =
      nlohmann::json::parse(std::ifstream{"test/test_heatmap_small/config.json"});
  controller.setWorkingDirectory("test/test_heatmap_small");
  sleep(2);
  controller.start(settings, controller.calcOptimalThreadNumber(settings, 0),
                   joda::helper::RandomNameGenerator::GetRandomName());
  int a = 0;
  std::cin >> a;
  controller.stop();
}
