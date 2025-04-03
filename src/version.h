///
/// \file      version.h
/// \author    Joachim Danmayr
/// \date      2023-04-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <sys/stat.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "build_info.h"

class Version
{
public:
  static auto getVersion() -> std::string
  {
    return VERSION;
  }

  static auto getBuildTime() -> std::string
  {
    return BUILD_TIME;
  }

  static void initVersion(const std::string &bin)
  {
  }

  static std::string getProgamName()
  {
    return "ImageC";
  }

  static std::string getTitle()
  {
    return "EVAnalyzer2";
  }

  static std::string getSubtitle()
  {
    return "powered by " + getProgamName();
  }

  static std::string getLogo()
  {
    std::string logo = "    _                            ______ \n";
    logo += "   (_)___ ___  ____ _____  ___  / ____/\n";
    logo += "  / / __ `__ \\/ __ `/ __ `/ _ \\/ /   \n";
    logo += " / / / / / / / /_/ / /_/ /  __/ /___   \n";
    logo += "/_/_/ /_/ /_/\\__,_/\\__, /\\___/\\____/\n";
    logo += "                  /____/               \n";
    logo += "_______________________________________\n";
    return logo;
  }
};
