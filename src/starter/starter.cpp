///
/// \file      command_line.cpp
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
#include "backend/updater/updater.hpp"
#include "backend/user_settings/user_settings.hpp"
#include "controller/controller.hpp"
#include "ui/cli/cli.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "version.h"

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
  // ======================================
  // Init application stuff
  // ======================================
  Version::initVersion(std::string(argv[0]));

  // ======================================
  // Load user settings
  // ======================================
  try {
    joda::user_settings::UserSettings::open();
    joda::log::logDebug("User settings loaded.");
  } catch(const std::exception &ex) {
    joda::log::logWarning("Could not open user settings! What: " + std::string(ex.what()));
  }

  // ===================================
  // Start CLI or GUI mode
  // ==================================
  if(argc <= 1) {
    startUi(argc, argv);
  } else {
    joda::ui::cli::Cli term;
    term.startCommandLineController(argc, argv);
  }

  joda::image::reader::ImageReader::destroy();
}

class NoIconStyle : public QProxyStyle
{
public:
  using QProxyStyle::QProxyStyle;

  QIcon standardIcon(StandardPixmap sp, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override
  {
    // Return empty icon only for button box standard buttons
    if((widget != nullptr) && (qobject_cast<const QDialogButtonBox *>(widget->parentWidget()) != nullptr)) {
      return QIcon();
    }
    return QProxyStyle::standardIcon(sp, option, widget);
  }
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int Starter::startUi(int argc, char *argv[])
{
  // Use QCoreApplication initially to parse command-line arguments
  // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  // QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  joda::ctrl::Controller::initApplication();
  auto controller = std::make_unique<joda::ctrl::Controller>();

  // ======================================
  // Check for updates thread
  // ======================================
  joda::updater::Updater updater;
  updater.triggerCheckForUpdates();

  // ======================================
  // Start QApplication
  // ======================================
  QApplication app(argc, argv);
  QApplication::setApplicationName(Version::getProgamName().data());
  QApplication::setApplicationVersion(Version::getVersion().data());

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

  // #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  //   qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
  //   QGuiApplication::setHighDpiScaleFactorRoundingPolicy(QetSettings::hdpiScaleFactorRoundingPolicy());
  // #endif

  // QFont font("Roboto");
  // app.setFont(font);

  QApplication::setStyle("Fusion");
  QApplication::setStyle(new NoIconStyle(QApplication::style()));

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
        QToolBar#MainWindowTopToolBar QToolButton {
          margin-right: 4px;
          padding: 4px;       
        }
        QToolBar#MainWindowTopToolBar {
          padding-top: 8px; 
          padding-bottom: 8px;
          padding-left: 4px; 
          padding-right: 4px;
        }
    )";

  app.setStyleSheet(stylesheet);
  mWindowMain = new joda::ui::gui::WindowMain(controller.get(), &updater);

  // ======================================
  // Show splash screen
  // ======================================
  QSplashScreen *splashScreen = nullptr;
  // if(runGui) {
  //   QPixmap pixmap(":/icons/icons/icon.png");
  //   splash = new QSplashScreen(pixmap);
  //   splash->show();
  //   QApplication::processEvents();
  // }

  if(nullptr != splashScreen) {
    splashScreen->finish(mWindowMain);    // Close the splash screen once done
  }
  mWindowMain->show();
  return QApplication::exec();
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
