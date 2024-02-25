#include <QtWidgets>
#include "controller/controller.hpp"
#include "ui/qt/window_main.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
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
      "   background-color: rgba(0, 0, 0, 0);"
      "   border: 1px solid rgb(111, 121, 123);"
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
      "}");

  // Option 1, like in the mentioned stackoverflow answer
  // QWidget window;

  auto *controller = new joda::ctrl::Controller();
  joda::ui::qt::WindowMain mainWindow(controller);

  mainWindow.show();
  return app.exec();
}
