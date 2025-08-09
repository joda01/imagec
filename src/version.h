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

  static auto getCudaEnabled() -> bool
  {
    return "True" == getCudaEnabledStr();
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
    if(getCudaEnabled()) {
      return "ImageC (CUDA)";
    }
    return "ImageC";
  }

  static std::string getSubtitle()
  {
    return "powered by " + getProgamName();
  }

  static std::string getLogo()
  {
    std::ostringstream oss;
    oss << "    ____                           ______ \n"
        << "   /  _/___ ___  ____ _____ ____  / ____/ \n"
        << "   / // __ `__ \\/ __ `/ __ `/ _ \\/ /      \n"
        << " _/ // / / / / / /_/ / /_/ /  __/ /___    \n"
        << "/___/_/ /_/ /_/\\__,_/\\__, /\\___/\\____/    \n"
        << "                    /____/                 \n";

    return oss.str();
  }

private:
#ifdef CUDA_ENABLED
  static auto getCudaEnabledStr() -> std::string
  {
    return CUDA_ENABLED;
  }
#else
  static auto getCudaEnabledStr() -> std::string
  {
    return "False";
  }
#endif
};
