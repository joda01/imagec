#include <QtWidgets>
#include "backend/duration_count/duration_count.h"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "backend/image_reader/tif/image_loader_tif.hpp"
#include "backend/pipelines/pipeline_factory.hpp"
#include "controller/controller.hpp"
#include "ui/window_main.hpp"
#include "version.h"

int main(int argc, char *argv[])
{
  //
  // Init
  //
  Version::initVersion(std::string(argv[0]));
  TiffLoader::initLibTif();
  BioformatsLoader::init();
  joda::pipeline::PipelineFactory::init();

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

  QFont font("Roboto");
  app.setFont(font);

  QApplication::setStyle("Fusion");
  app.setStyleSheet(
      "QMainWindow#windowMain {"
      "   background-color: rgb(251, 252, 253); "
      "   border: none;"
      "}"

      "QMessageBox {"
      "   background-color: rgb(251, 252, 253);"
      "}"

      "QMessageBox QLabel {"
      "   color: #000;"
      "   font-size: 16px;"
      "   padding-bottom: 20px;"
      "   padding-top: 20px;"
      "}"

      "QMessageBox QPushButton {"
      "   background-color: rgb(255, 255, 255);"
      "   border: 1px solid rgb(0, 0, 0);"
      "   color: #000;"
      "   padding: 10px 20px;"
      "   border-radius: 12px;"
      "   font-size: 14px;"
      "   font-weight: normal;"
      "   text-align: center;"
      "   text-decoration: none;"
      "}"

      "QMessageBox QPushButton#qt_msgbox_buttonrole {"
      "    padding-left: 10px;"        // Add some left padding to adjust the text position
      "    padding-right: 10px;"       // Add some right padding to adjust the text position
      "    border: 1px solid #ccc;"    // Optional: Add border for a cleaner look
      "}"

      "QMessageBox QPushButton:hover {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on hover
      "}"

      "QMessageBox QPushButton:pressed {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on press
      "}"

      "QMessageBox QPushButton:default {"
      "   border: 2px solid #2196F3;"
      "}"
      /*Push button*/
      "QPushButton {"
      "   border: 1px solid rgb(0, 0, 0);"
      "   background-color: rgb(255, 255, 255);"
      "   color: rgb(0, 0, 0);"
      "   border-radius: 4px;"
      "   padding-top: 10px;"
      "   padding-bottom: 10px;"
      "   font-size: 14px;"
      "   font-weight: normal;"
      "   text-decoration: none;"
      "   text-align: center;"
      "}"

      "QPushButton:hover {"
      "   background-color: rgb(255, 255, 255);"    // Darken on hover
      "}"

      "QPushButton:pressed {"
      "   background-color: rgb(255, 255, 255);"    // Darken on press
      "}"

      /*Combo box*/
      "QComboBox {"
      "   border: 1px solid rgb(0, 0, 0);"
      "   background-color: rgb(255, 255, 255);"
      "   color: rgb(0, 0, 0);"
      "   border-radius: 4px;"
      "   padding-top: 10px;"
      "   padding-bottom: 10px;"
      "   padding-left: 10px;"
      "   font-size: 14px;"
      "   font-weight: normal;"
      "   text-decoration: none;"
      "   selection-background-color: rgb(255, 247, 241);"
      "}"
      "QComboBox:editable {"
      "   background: #fff;"
      "   padding-left: 20px;"
      "}"

      "QComboBox::drop-down {"
      "   subcontrol-origin: padding;"
      "   subcontrol-position: right top;"
      "   width: 20px;"
      "   border-left: none;"
      "   border-radius: 4px 4px 4px 4px;"
      "   background: #fff;"
      "   padding-top: 10px;"
      "   padding-bottom: 10px;"
      "}"

      "QComboBox::down-arrow {"
      "   image: url(:/icons/outlined/icons8-sort-down-50.png);"
      "   width: 14px;"
      "   background: #fff;"
      "}"

      "QComboBox::down-arrow:on {"
      "   top: 1px;"
      "}"

      "QComboBox QAbstractItemView {"
      "   border: none;"
      "   background-color: #fff;"
      "}"

      /*Text field box*/
      "QLineEdit { border-radius: 4px; border: 1px solid rgb(0, 0, 0); padding-top: 10px; padding-bottom: "
      "10px;}"

  );

  // Option 1, like in the mentioned stackoverflow answer
  // QWidget window;

  auto *controller = new joda::ctrl::Controller();
  joda::ui::qt::WindowMain mainWindow(controller);

  mainWindow.show();

  auto ret = app.exec();
  joda::pipeline::PipelineFactory::shutdown();
  BioformatsLoader::destroy();
  return ret;
}
