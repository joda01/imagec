///
/// \file      program_meta.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ProgramMeta, imagecVersion);
};

}    // namespace joda::settings
