///
/// \file      navigation.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

///
/// \class      Navigation
/// \author     Joachim Danmayr
/// \brief      Menu navigation
///
#include <string>
class Navigation
{
public:
  /////////////////////////////////////////////////////
  Navigation();

  void start();

private:
  /////////////////////////////////////////////////////
  auto readFromConsole(int y, const std::string &desc, const std::string &def) -> std::string;
  static int printLogo();
  void menuMain();
  auto menuGetInputFolder() -> std::string;
  void menuStartAnalyzes();
  void menuReportResult();

  void drawProgressBar(int x, int y, int width, float act, float total);
  void saveUserSettings();
  void loadUserSettings();

  static void clearScreen();
  void cancelReadFromConsole()
  {
    mReadFromConsoleCanceled = true;
  }

  std::string mSelectedInputFolder;
  std::string mLastReport;
  bool mReadFromConsoleCanceled = false;
};
