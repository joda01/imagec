///
/// \file      command_line.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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
