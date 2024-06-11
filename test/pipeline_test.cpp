#include <filesystem>
#include <thread>
#include "backend/helper/file_info_images.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/image_processing/reader/bioformats/bioformats_loader.hpp"
#include "backend/image_processing/reader/jpg/image_loader_jpg.hpp"
#include "backend/pipelines/pipeline_factory.hpp"
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/settings/analze_settings.hpp"
#include "controller/controller.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json_fwd.hpp>

using namespace std::chrono_literals;

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
SCENARIO("pipeline:test:heatmap", "[pipeline]")
{
  GIVEN("A test set of two images")
  {
    joda::image::BioformatsLoader::init();
    joda::ctrl::Controller controller;

    WHEN("Executing the pipeline")
    {
      joda::settings::AnalyzeSettings settings =
          nlohmann::json::parse(std::ifstream{"test/scenarios/pipeline_01.json"});
      controller.setWorkingDirectory("test/test_data/images/pipeline_01");
      std::this_thread::sleep_for(2s);
      controller.start(settings, controller.calcOptimalThreadNumber(settings, 0),
                       joda::helper::RandomNameGenerator::GetRandomName());

      while(true) {
        std::this_thread::sleep_for(2s);
        auto [_, state, str] = controller.getState();
        if(state == joda::pipeline::Pipeline::State::FINISHED) {
          break;
        }
      }
      controller.stop();
      THEN("We expect a created database with filled out data.")
      {
        auto databaseFile = std::filesystem::path(controller.getOutputFolder()) / "results.duckdb";
        joda::results::Analyzer results(databaseFile);
      }
    }
  }
}
