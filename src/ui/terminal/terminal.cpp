///
/// \file      terminal.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "terminal.hpp"
#include <exception>
#include <string>
#include <thread>
#include "backend/database/query/filter.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/settings/settings.hpp"
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
    analyzeSettings = joda::settings::Settings::openSettings(pathToSettingsFile);
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
  settings::ResultsSettings filter;

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

  exporter::xlsx::ExportSettings::ExportSettings::ExportType typeEnum;
  if(type == "xlsx") {
    typeEnum = exporter::xlsx::ExportSettings::ExportSettings::ExportType::XLSX;
  } else if(type == "r") {
    typeEnum = exporter::xlsx::ExportSettings::ExportType::R;
  } else {
    joda::log::logError("Invalid export type!");
    std::exit(1);
  }

  exporter::xlsx::ExportSettings::ExportFormat formatEnum;
  if(format == "list") {
    formatEnum = exporter::xlsx::ExportSettings::ExportFormat::LIST;
  } else if(format == "heatmap") {
    formatEnum = exporter::xlsx::ExportSettings::ExportFormat::HEATMAP;
  } else {
    joda::log::logError("Invalid export format!");
    std::exit(1);
  }

  auto filterElements = joda::helper::split(exportFilter, {' '});

  exporter::xlsx::ExportSettings::ExportView viewEnum;
  if(view == "plate") {
    viewEnum = exporter::xlsx::ExportSettings::ExportView::PLATE;
    if(filterElements.size() < 1) {
      joda::log::logError("Export filter in form [plate-id] must be given!");
      std::exit(1);
    }
  } else if(view == "well") {
    viewEnum = exporter::xlsx::ExportSettings::ExportView::WELL;
    if(filterElements.size() < 2) {
      joda::log::logError("Export filter in form [plate-id group-id] must be given!");
      std::exit(1);
    }
  } else if(view == "image") {
    viewEnum = exporter::xlsx::ExportSettings::ExportView::IMAGE;
    if(filterElements.size() < 3) {
      joda::log::logError("Export filter in form [t-stack, plate-id group-id image-file-name] must be given!");
      std::exit(1);
    }
  } else {
    joda::log::logError("Invalid export view!");
    std::exit(1);
  }

  int32_t tStack  = 0;
  int32_t plateId = 0;
  int32_t groupId = 0;
  std::string imageFileName;
  try {
    tStack  = std::stoi(filterElements[0]);
    plateId = std::stoi(filterElements[1]);
    if(filterElements.size() > 2) {
      groupId = std::stoi(filterElements[2]);
    }
    if(filterElements.size() > 3) {
      imageFileName = filterElements[3];
    }
  } catch(const std::exception &e) {
    joda::log::logError("Plate ID and Group ID must be a number between [0-65535].");
    std::exit(1);
  }

  try {
    mController->exportData(pathToDatabasefile, filter,
                            joda::exporter::xlsx::ExportSettings{formatEnum, typeEnum, viewEnum, {plateId, groupId, tStack, imageFileName}},
                            outputPath);
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    std::exit(1);
  }
  std::exit(0);
}

}    // namespace joda::ui::terminal
