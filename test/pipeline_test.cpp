#include "backend/helper/file_info_images.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/image_processing/reader/bioformats/bioformats_loader.hpp"
#include "backend/image_processing/reader/jpg/image_loader_jpg.hpp"
#include "backend/pipelines/pipeline_factory.hpp"
#include "backend/settings/analze_settings.hpp"
#include "controller/controller.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json_fwd.hpp>

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("pipeline:test:heatmap_small", "[.][pipeline_test_heatmap_small]")
{
  joda::image::BioformatsLoader::init();
  joda::ctrl::Controller controller;

  joda::settings::AnalyzeSettings settings =
      nlohmann::json::parse(std::ifstream{"test/test_heatmap_small/config.json"});
  controller.setWorkingDirectory("test/test_heatmap_small");
  sleep(2);
  controller.start(settings, controller.calcOptimalThreadNumber(settings, 0),
                   joda::helper::RandomNameGenerator::GetRandomName());

  while(true) {
    sleep(2);
    auto [_, state, str] = controller.getState();
    if(state == joda::pipeline::Pipeline::State::FINISHED) {
      break;
    }
  }
  controller.stop();
}
