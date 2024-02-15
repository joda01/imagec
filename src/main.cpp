#include <QtWidgets>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // Option 1, like in the mentioned stackoverflow answer
  // QWidget window;

  // Option 2, to test If maybe the QWidget above was the problem
  QMainWindow window;
  window.setWindowTitle("Test");

  window.show();
  return app.exec();
}
