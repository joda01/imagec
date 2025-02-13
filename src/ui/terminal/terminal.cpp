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
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/helper.hpp"
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
void Terminal::exportData(const std::filesystem::path &pathToDatabasefile, const std::filesystem::path &outputPath,
                          const std::filesystem::path &pathToQueryFilter, const std::string &type, const std::string &format, const std::string &view,
                          const std::string &exportFilter)
{
  db::QueryFilter filter;

  // ==========================
  // Open settings file
  // ==========================
  try {
    std::ifstream ifs(pathToQueryFilter.string());
    filter = nlohmann::json::parse(ifs);
    ifs.close();
  } catch(const std::exception &ex) {
    joda::log::logError("Could not load filter file >" + std::string(ex.what()) + "<!");
    std::exit(1);
  }

  ctrl::ExportSettings::ExportType typeEnum;
  if(type == "xlsx") {
    typeEnum = ctrl::ExportSettings::ExportType::XLSX;
  } else if(type == "r") {
    typeEnum = ctrl::ExportSettings::ExportType::R;
  } else {
    joda::log::logError("Invalid export type!");
    std::exit(1);
  }

  ctrl::ExportSettings::ExportFormat formatEnum;
  if(format == "list") {
    formatEnum = ctrl::ExportSettings::ExportFormat::LIST;
  } else if(format == "heatmap") {
    formatEnum = ctrl::ExportSettings::ExportFormat::HEATMAP;
  } else {
    joda::log::logError("Invalid export format!");
    std::exit(1);
  }

  ctrl::ExportSettings::ExportView viewEnum;
  if(view == "plate") {
    viewEnum = ctrl::ExportSettings::ExportView::PLATE;
  } else if(view == "well") {
    viewEnum = ctrl::ExportSettings::ExportView::WELL;
  } else if(view == "image") {
    viewEnum = ctrl::ExportSettings::ExportView::IMAGE;
  } else {
    joda::log::logError("Invalid export view!");
    std::exit(1);
  }

  auto filterElements = joda::helper::split(exportFilter, {' '});
  if(filterElements.size() != 3) {
    joda::log::logError("Export filter in form [plate-id group-id image-id] must be given!");
    std::exit(1);
  }

  try {
    int32_t plateId  = std::stoi(filterElements[0]);
    int32_t groupId  = std::stoi(filterElements[1]);
    uint64_t imageId = std::stoull(filterElements[2]);

    filter.setFilter(plateId, groupId, imageId);
  } catch(const std::exception &e) {
    joda::log::logError("Export filter in form [plate-id group-id image-id] of three numbers must be given!");
    std::exit(1);
  }

  joda::log::logInfo("Export started!");
  mController->exportData(pathToDatabasefile, filter, joda::ctrl::ExportSettings{formatEnum, typeEnum, viewEnum}, outputPath);
  joda::log::logInfo("Export finished!");
}

}    // namespace joda::ui::terminal
