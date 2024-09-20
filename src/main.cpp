#include <QtWidgets>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/reader/image_reader.hpp"
#include "controller/controller.hpp"
#include "ui/window_main/window_main.hpp"
#include "version.h"

int main(int argc, char *argv[])
{
  //
  // Init
  //
  Version::initVersion(std::string(argv[0]));
  joda::image::reader::ImageReader::init();

#ifdef _WIN32
#elif defined(__APPLE__)
#else
  //  setenv("LD_LIBRARY_PATH", "./lib", 1);
  setenv("FONTCONFIG_PATH", "/etc/fonts/", 1);
#endif

  //
  // Start UI
  //
  QApplication app(argc, argv);

  // Load the Roboto font file
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

  // Apply the stylesheet
  app.setStyleSheet(stylesheet);

  auto *controller = new joda::ctrl::Controller();
  joda::ui::WindowMain mainWindow(controller);

  mainWindow.show();

  auto ret = app.exec();
  joda::image::reader::ImageReader::destroy();
  return ret;
}
