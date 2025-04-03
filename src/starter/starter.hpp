///
/// \file      command_line.hpp
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

#pragma once

#include <qapplication.h>
#include <qsplashscreen.h>
#include <memory>
#include <string>
#include <thread>

namespace joda::ctrl {
class Controller;
};

namespace joda::ui::gui {
class WindowMain;

}

namespace joda::start {

class Starter
{
public:
  Starter();
  void exec(int argc, char *argv[]);

private:
  /////////////////////////////////////////////////////
  void initLogger(const std::string &logLevel);
  auto initApplication() -> std::shared_ptr<std::thread>;
  void startUi(QApplication &app, QSplashScreen *splashScreen);
  void exitWithError(const std::string &what);

  ctrl::Controller *mController;
  joda::ui::gui::WindowMain *mWindowMain;
};

}    // namespace joda::start
