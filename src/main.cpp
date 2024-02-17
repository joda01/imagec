#include <QtWidgets>
#include "ui/qt/window_main.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // Option 1, like in the mentioned stackoverflow answer
  // QWidget window;

  joda::ui::qt::WindowMain mainWindow;

  mainWindow.show();
  return app.exec();
}
