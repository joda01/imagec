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
  joda::log::updateTopLine(0, 100);
  auto jobName = joda::helper::RandomNameGenerator::GetRandomName();
  mController->start(analyzeSettings, {}, jobName);
  joda::log::logInfo("Job >" + jobName + "< started!");

  // ==========================
  // Running
  // ==========================
  int32_t totalTiles    = 0;
  int32_t finishedTiles = 0;
  try {
    while(true) {
      const auto &jobState = mController->getState();
      if(jobState.isFinished()) {
        break;
      }

      finishedTiles = jobState.finishedTiles();
      totalTiles    = jobState.totalTiles();
      joda::log::updateTopLine(finishedTiles, totalTiles);
      std::this_thread::sleep_for(1s);
    }
  } catch(...) {
  }
  joda::log::logInfo("Job >" + jobName + "< finished!");
  joda::log::updateTopLine(totalTiles, totalTiles);
  joda::log::setConsoleLog(false);

  std::exit(0);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Terminal::printProgress()
{
}

}    // namespace joda::ui::terminal
