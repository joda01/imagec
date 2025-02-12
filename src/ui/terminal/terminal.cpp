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
void Terminal::startAnalyze(const std::filesystem::path &pathToSettingsFile, std::optional<std::string> &imagedInputFolder)
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
  // Prepare and check settings
  // ==========================
  auto foundErrors = analyzeSettings.checkForErrors();
  bool hasError    = false;
  for(const auto &[pipeline, messages] : foundErrors) {
    for(const auto &msg : messages) {
      if(msg.severity == SettingParserLog::Severity::JODA_ERROR) {
        hasError = true;
      }
      msg.print();
    }
  }
  if(hasError) {
    joda::log::logError("Configuration has errors!");
    std::exit(0);
  }

  if(imagedInputFolder.has_value()) {
    analyzeSettings.projectSettings.workingDirectory            = imagedInputFolder.value();
    analyzeSettings.projectSettings.plates.begin()->imageFolder = imagedInputFolder.value();
  }

  // ==========================
  // Start job
  // ==========================
  auto jobName = joda::helper::RandomNameGenerator::GetRandomName();
  mController->start(analyzeSettings, {}, jobName);
  joda::log::logInfo("Job >" + jobName + "< started!");

  // ==========================
  // Running
  // ==========================
  float totalTiles    = 0;
  float finishedTiles = 0;
  try {
    while(true) {
      std::this_thread::sleep_for(2.5s);

      const auto &jobState = mController->getState();
      if(jobState.isFinished()) {
        break;
      }

      finishedTiles = static_cast<float>(jobState.finishedTiles());
      totalTiles    = static_cast<float>(jobState.totalTiles());
      if(totalTiles > 0) {
        joda::log::logProgress(finishedTiles / totalTiles, "Analyze running");
      } else {
        joda::log::logProgress(0, "Progress");
      }
    }
  } catch(...) {
  }
  joda::log::logProgress(1, "Completed");
  joda::log::logInfo("Job >" + jobName + "< finished!");
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
