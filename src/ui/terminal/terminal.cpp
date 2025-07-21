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
#include <memory>
#include <string>
#include <thread>
#include "backend/database/query/filter.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/settings/settings.hpp"
#include "controller/controller.hpp"
#include <CLI/CLI.hpp>

namespace joda::ui::terminal {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Terminal::Terminal()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int Terminal::startCommandLineController(int argc, char *argv[])
{
  CLI::App app{Version::getTitle()};

  std::string input, settings;
  auto run = app.add_subcommand("run", "Run the main process");
  run->add_option("--input", input, "Input file")->required();
  run->add_option("--settings", settings, "Settings file")->required();

  std::string infile, outfile, options;
  auto export_cmd = app.add_subcommand("export", "Export processed data");
  export_cmd->add_option("--infile", infile, "Input file")->required();
  export_cmd->add_option("--outfile", outfile, "Output file")->required();
  export_cmd->add_option("--options", options, "Export options file")->required();

  CLI11_PARSE(app, argc, argv);

  joda::ctrl::Controller::initApplication();
  mController = std::make_unique<joda::ctrl::Controller>();

  if(run->parsed()) {
    // Run logic
  } else if(export_cmd->parsed()) {
    // Export logic
  }

  return 0;

  /*
  // ======================================
  // Init command line parser
  // ======================================
  QCommandLineParser parser;
  parser.setApplicationDescription("ImageC high throughput image processing application.");
  parser.addHelpOption();
  parser.addVersionOption();

  //
  // Common commands
  //

  // Add an option for CLI to accept a name
  QCommandLineOption loggingOption(QStringList() << "l"
                                                 << "logging",
                                   "Set logging level (error, warning, info, debug, trace).", "loglevel");
  parser.addOption(loggingOption);

  //
  // Run analyze commends
  //

  // Add run option
  QCommandLineOption runOption(QStringList() << "r"
                                             << "run",
                               "Start an analyze.", "*.icproj");
  parser.addOption(runOption);

  // Add path options
  QCommandLineOption imagePathOption(QStringList() << "p"
                                                   << "run-path",
                                     "Path to images which should be analyzed.", "folder");
  parser.addOption(imagePathOption);

  //
  // Export commands
  //
  QCommandLineOption exportData(QStringList() << "e"
                                              << "export",
                                "Export data form a run.", "*.icdb");
  parser.addOption(exportData);

  QCommandLineOption resultsOutput(QStringList() << "o"
                                                 << "export-output",
                                   "Path and filename to store the exported data.", "path/filename");
  parser.addOption(resultsOutput);

  QCommandLineOption queryFilterTemplate(QStringList() << "c"
                                                       << "export-columns",
                                         "Path to results table settings file.", "*." + QString(joda::fs::EXT_CLASS_CLASS_TEMPLATE.data()) + "");
  parser.addOption(queryFilterTemplate);

  QCommandLineOption exportType(QStringList() << "t"
                                              << "export-type",
                                "Export either R or XLSX (r, xlsx)", "type");
  parser.addOption(exportType);

  QCommandLineOption exportFormat(QStringList() << "f"
                                                << "export-format",
                                  "Export either list or heatmap (list, heatmap)", "format");
  parser.addOption(exportFormat);

  QCommandLineOption exportView(QStringList() << "w"
                                              << "export-view",
                                "Which view should be exported (plate, well, image)", "view");
  parser.addOption(exportView);

  QCommandLineOption exportFilter(QStringList() << "q"
                                                << "export-filter",
                                  "Plate, group and image to export [plate-id group-id image-id]", "filter");
  parser.addOption(exportFilter);

  parser.process(app);

  // ===================================
  // Logger
  // ==================================
  if(parser.isSet(loggingOption)) {
    QString loglevel = parser.value(loggingOption);
    initLogger(loglevel.toStdString());
  } else {
    initLogger("trace");
  }

  bool runGui = !parser.isSet(runOption) && !parser.isSet(exportData);

  // ===================================
  // Run analyze
  // ==================================
  if(parser.isSet(runOption)) {
    std::optional<std::string> imageInputPath = std::nullopt;
    if(parser.isSet(imagePathOption)) {
      imageInputPath = parser.value(imagePathOption).toStdString();
    }

    QString settingsFilePath = parser.value(runOption);
    joda::ui::terminal::Terminal terminal(mController);
    terminal.startAnalyze(std::filesystem::path(settingsFilePath.toStdString()), imageInputPath);
  }

  // ===================================
  // Export
  // ==================================
  if(parser.isSet(exportData)) {
    joda::ui::terminal::Terminal terminal(mController);
    terminal.exportData(parser.value(exportData).toStdString(), parser.value(resultsOutput).toStdString(),
                        parser.value(queryFilterTemplate).toStdString(), parser.value(exportType).toStdString(),
                        parser.value(exportFormat).toStdString(), parser.value(exportView).toStdString(), parser.value(exportFilter).toStdString());
  }
                        */
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
    analyzeSettings.projectSettings.workingDirectory  = imagedInputFolder.value();
    analyzeSettings.projectSettings.plate.imageFolder = imagedInputFolder.value();
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
void Terminal::exportData(const std::filesystem::path &pathToDatabasefile, const std::filesystem::path &outputPath, const std::string &type,
                          const std::string &format, const std::string &view, const std::string &exportFilter,
                          const std::filesystem::path &pathToQueryFilter)
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

///
/// \brief      Init logger
///             allowed inputs are: [off, error, warning, info, debug, trace]
/// \author     Joachim Danmayr
///
void Terminal::initLogger(const std::string &logLevel)
{
  if(logLevel == "off") {
    joda::log::setLogLevel(joda::log::LogLevel::OFF);
  } else if(logLevel == "error") {
    joda::log::setLogLevel(joda::log::LogLevel::ERROR_);

  } else if(logLevel == "warning") {
    joda::log::setLogLevel(joda::log::LogLevel::WARNING);

  } else if(logLevel == "info") {
    joda::log::setLogLevel(joda::log::LogLevel::INFO);

  } else if(logLevel == "debug") {
    joda::log::setLogLevel(joda::log::LogLevel::DEBUG);

  } else if(logLevel == "trace") {
    joda::log::setLogLevel(joda::log::LogLevel::TRACE);
  } else {
    std::cout << "Wrong parameter for loglevel!" << std::endl;
    std::exit(1);
  }
}

}    // namespace joda::ui::terminal
