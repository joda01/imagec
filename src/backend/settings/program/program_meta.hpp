///
/// \file      program_meta.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <string>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>
#include "version.h"

namespace joda::settings {

struct ProgramMeta final
{
  std::string imagecVersion = Version::getVersion();
  std::string buildTime     = Version::getBuildTime();

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ProgramMeta, imagecVersion, buildTime);
};

}    // namespace joda::settings
