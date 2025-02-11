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
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/system/system_resources.hpp"
#include "controller/controller.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include "ui/terminal/terminal.hpp"
#include "version.h"

void cleanup()
{
  joda::image::reader::ImageReader::destroy();
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
  Version::initVersion(std::string(argv[0]));
  // Use QCoreApplication initially to parse command-line arguments
  QApplication app(argc, argv);
  QApplication::setApplicationName(Version::getProgamName().data());
  QApplication::setApplicationVersion(Version::getVersion().data());

  QCommandLineParser parser;
  parser.setApplicationDescription("ImageC high throughput image processing application.");
  parser.addHelpOption();
  parser.addVersionOption();

  // Add a command-line option to trigger CLI mode
  QCommandLineOption cliOption(QStringList() << "c"
                                             << "cli",
                               "Run in command-line mode.");

  parser.addOption(cliOption);

  // Add an option for CLI to accept a name
  QCommandLineOption loggingOption(QStringList() << "l"
                                                 << "logging",
                                   "Set logging level [off, error, warning, info, debug, trace]", "loglevel");
  parser.addOption(loggingOption);

  // Add run option
  QCommandLineOption runOption(QStringList() << "r"
                                             << "run",
                               "Start an analyze.", "settings file");
  parser.addOption(runOption);

  parser.process(app);

  // ===================================
  // Logger
  // ==================================
  if(parser.isSet(loggingOption)) {
    QString loglevel = parser.value(loggingOption);
    initLogger(loglevel.toStdString());
  }

  // ===================================
  // Init application
  // ==================================
  initApplication();

  // ===================================
  // Run analyze
  // ==================================
  if(parser.isSet(runOption)) {
    QString settingsFilePath = parser.value(runOption);
    joda::ui::terminal::Terminal terminal(mController);
    terminal.startAnalyze(std::filesystem::path(settingsFilePath.toStdString()));
  }

  // ===================================
  // Start CLI or GUI mode
  // ==================================
  if(parser.isSet(cliOption)) {
    qDebug() << "Running in Command-Line Mode";
    std::exit(0);    // Exit after CLI execution
  } else {
    startUi(app);
  }

  QApplication::exec();
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
    joda::log::setLogLevel(joda::log::LogLevel::ERROR);

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
void Starter::initApplication()
{
  // ======================================
  // Reserve system resources
  // ======================================
  auto systemRecourses   = joda::system::acquire();
  int32_t totalRam       = std::ceil(static_cast<float>(systemRecourses.ramTotal) / 1000000.0f);
  int32_t availableRam   = std::ceil(static_cast<float>(systemRecourses.ramAvailable) / 1000000.0f);
  int32_t jvmReservedRam = std::ceil(static_cast<float>(systemRecourses.ramReservedForJVM) / 1000000.0f);

  joda::log::logDebug("Total available RAM " + std::to_string(totalRam) + " MB.");
  joda::log::logDebug("Usable RAM " + std::to_string(availableRam) + " MB.");
  joda::log::logDebug("JVM reserved RAM " + std::to_string(jvmReservedRam) + " MB.");

  joda::image::reader::ImageReader::init(systemRecourses.ramReservedForJVM);

  // Register the cleanup function
  if(atexit(cleanup) != 0) {
    std::cerr << "Failed to register cleanup function." << std::endl;
    std::exit(1);    // Exit after CLI execution
  }
  mController = new joda::ctrl::Controller();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Starter::startUi(QApplication &app)
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
