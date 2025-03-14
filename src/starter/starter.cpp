///
/// \file      command_line.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "starter.hpp"
#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <memory>
#include <optional>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/system/system_resources.hpp"
#include "controller/controller.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include "ui/terminal/terminal.hpp"
#include "version.h"

void cleanup()
{
}

namespace joda::start {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Starter::Starter()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Starter::exec(int argc, char *argv[])
{
  // Use QCoreApplication initially to parse command-line arguments
  QApplication app(argc, argv);

  // ======================================
  // Init application stuff
  // ======================================
  Version::initVersion(std::string(argv[0]));
  QApplication::setApplicationName(Version::getProgamName().data());
  QApplication::setApplicationVersion(Version::getVersion().data());

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
                                         "Path to column export template file.", "*.ictemplexp");
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
  // ======================================
  // Show splash screen
  // ======================================
  QSplashScreen *splash = nullptr;
  // if(runGui) {
  //   QPixmap pixmap(":/icons/icons/icon.png");
  //   splash = new QSplashScreen(pixmap);
  //   splash->show();
  //   QApplication::processEvents();
  // }

  // ===================================
  // Init application
  // ==================================
  auto initFuture = initApplication();

  if(!runGui) {
    // If we ar not running in GUI mode we have to wait for the init finished before we can start.
    initFuture->join();
  }

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

  // ===================================
  // Start CLI or GUI mode
  // ==================================
  if(runGui) {
    startUi(app, splash);
  }

  QApplication::exec();

  if(initFuture->joinable()) {
    initFuture->join();
  }

  joda::image::reader::ImageReader::destroy();
}

///
/// \brief      Init logger
///             allowed inputs are: [off, error, warning, info, debug, trace]
/// \author     Joachim Danmayr
///
void Starter::initLogger(const std::string &logLevel)
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
    exitWithError("Wrong parameter for loglevel!");
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Starter::initApplication() -> std::shared_ptr<std::thread>
{
  // ======================================
  // Reserve system resources
  // ======================================
  auto systemRecourses   = joda::system::acquire();
  int32_t totalRam       = std::ceil(static_cast<float>(systemRecourses.ramTotal) / 1000000.0f);
  int32_t availableRam   = std::ceil(static_cast<float>(systemRecourses.ramAvailable) / 1000000.0f);
  int32_t jvmReservedRam = std::ceil(static_cast<float>(systemRecourses.ramReservedForJVM) / 1000000.0f);

  joda::log::logInfo("Total available RAM " + std::to_string(totalRam) + " MB.");
  joda::log::logInfo("Usable RAM " + std::to_string(availableRam) + " MB.");
  joda::log::logInfo("JVM reserved RAM " + std::to_string(jvmReservedRam) + " MB.");

  std::shared_ptr<std::thread> initJVMThread =
      std::make_shared<std::thread>([ram = systemRecourses.ramReservedForJVM] { /* joda::image::reader::ImageReader::init(ram); */ });
  joda::image::reader::ImageReader::init(systemRecourses.ramReservedForJVM);

  // Register the cleanup function
  if(atexit(cleanup) != 0) {
    std::cerr << "Failed to register cleanup function." << std::endl;
    std::exit(1);    // Exit after CLI execution
  }
  mController = new joda::ctrl::Controller();
  return initJVMThread;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Starter::startUi(QApplication &app, QSplashScreen *splashScreen)
{
  // ======================================
  // Start UI
  // ======================================
  QFile fontFile(":/fonts/fonts/roboto/Roboto-Regular.ttf");
  if(fontFile.open(QIODevice::ReadOnly)) {
    QByteArray fontData = fontFile.readAll();
    QFontDatabase::addApplicationFontFromData(fontData);
    fontFile.close();
  } else {
    qWarning() << "Failed to load font file";
  }

  // QFont font("Roboto");
  // app.setFont(font);

  QApplication::setStyle("Fusion");

  QString stylesheet = R"(
        QLineEdit {
            min-height: 26px;
        }
        QComboBox {
            min-height: 26px;
        }
        QComboBox QAbstractItemView {
            min-height: 26px;
        }
        QPushButton {
            min-height: 26px;
        }
        QWidget#PanelChannelOverview {
          border-radius: 0px;
          border-bottom: 1px solid rgb(170, 170, 170);
          padding-top: 10px;
          padding-bottom: 10px;
        }
    )";

  app.setStyleSheet(stylesheet);
  mWindowMain = new joda::ui::gui::WindowMain(mController);
  if(nullptr != splashScreen) {
    splashScreen->finish(mWindowMain);    // Close the splash screen once done
  }
  mWindowMain->show();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Starter::exitWithError(const std::string &what)
{
  std::cout << what << std::endl;
  std::exit(1);
}

}    // namespace joda::start
