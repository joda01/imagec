#include <unistd.h>
#include <string>
#include "backend/pipelines/pipeline.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include "controller.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("controller:test", "[optimal_thread_number_tile]")
{
  joda::ctrl::Controller ctrl;
  ctrl.setWorkingDirectory("test_nucleus");
  sleep(1);

  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test_nucleus/config.json"});
  auto result                              = ctrl.calcOptimalThreadNumber(settings, 0);

  std::cout << "Img: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::IMAGES])
            << " | Chs: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::CHANNELS])
            << " |Tils: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::TILES])
            << std::endl;
}

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("controller:test", "[optimal_thread_number_img]")
{
  joda::ctrl::Controller ctrl;
  ctrl.setWorkingDirectory("test_areosold_Evs");
  sleep(1);

  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{"test_areosold_Evs/settings.json"});
  auto result                              = ctrl.calcOptimalThreadNumber(settings, 0);

  std::cout << "Img: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::IMAGES])
            << " | Chs: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::CHANNELS])
            << " |Tils: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::TILES])
            << std::endl;
}

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("controller:test", "[optimal_thread_number_img_multi]")
{
  joda::ctrl::Controller ctrl;
  ctrl.setWorkingDirectory("test_spot/evanalyzer_comp");
  sleep(1);

  joda::settings::AnalyzeSettings settings =
      nlohmann::json::parse(std::ifstream{"test_spot/evanalyzer_comp/mysettings.json"});
  auto result = ctrl.calcOptimalThreadNumber(settings, 0);

  std::cout << "Img: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::IMAGES])
            << " | Chs: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::CHANNELS])
            << " |Tils: " << std::to_string(result.cores[joda::pipeline::Pipeline::ThreadingSettings::TILES])
            << std::endl;
}
