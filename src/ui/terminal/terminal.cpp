///
/// \file      terminal.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "terminal.hpp"
#include <exception>
#include <string>
#include <thread>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "controller/controller.hpp"

namespace joda::ui::terminal {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Terminal::Terminal(ctrl::Controller *controller) : mController(controller)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Terminal::startAnalyze(const std::filesystem::path &pathToSettingsFile)
{
  joda::settings::AnalyzeSettings analyzeSettings;

  // ==========================
  // Open settings file
  // ==========================
  try {
    std::ifstream ifs(pathToSettingsFile.string());
    analyzeSettings = nlohmann::json::parse(ifs);
    ifs.close();
  } catch(const std::exception &ex) {
    joda::log::logError("Could not load settings file >" + std::string(ex.what()) + "<!");
    std::exit(1);
  }

  // ==========================
  // Start job
  // ==========================
  auto jobName        = joda::helper::RandomNameGenerator::GetRandomName();
  auto threadSettings = mController->calcOptimalThreadNumber(analyzeSettings);
  mController->start(analyzeSettings, threadSettings, jobName);
  joda::log::logInfo("Job >" + jobName + "< started!");

  // ==========================
  // Running
  // ==========================
  try {
    std::string oldState;
    while(true) {
      const auto &jobState = mController->getState();
      if(jobState.isFinished()) {
        break;
      }

      float persentFinished = ((float) jobState.finishedTiles() / (float) jobState.totalTiles()) * 100.0;

      std::string newStet = ("Progress: " + std::to_string(persentFinished));
      if(newStet != oldState) {
        oldState = newStet;
        std::cout << newStet << std::endl;
      }

      std::this_thread::sleep_for(1s);
    }
  } catch(...) {
  }
  joda::log::logInfo("Job >" + jobName + "< finished!");
  std::exit(0);
}

}    // namespace joda::ui::terminal
