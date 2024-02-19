#include <QtWidgets>
#include "ui/qt/window_main.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setStyle("Fusion");

  // Option 1, like in the mentioned stackoverflow answer
  // QWidget window;

  joda::ui::qt::WindowMain mainWindow;

  mainWindow.show();
  return app.exec();
}

/*
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  QCoreApplication::addLibraryPath("./");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  return app.exec();
}
*/
