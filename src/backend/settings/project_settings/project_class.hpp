///
/// \file      project_object_class.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#pragma once

#include <set>
#include <vector>
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

static inline const std::vector<std::string> COLORS = {
    "#FF5733",    // - Vibrant Orange
    "#33FF57",    // - Bright Green
    "#FF33FF",    // - Magenta
    "#33A1FF",    // - Sky Blue
    "#FFFF33",    // - Bright Yellow
    "#FF33A1",    // - Hot Pink
    "#33FFD1",    // - Aqua
    "#FF8C33",    // - Deep Orange
    "#9933FF",    // - Purple
    "#33FFCC",    // - Mint Green
    "#FF3380",    // - Electric Pink
    "#33FF88",    // - Neon Green
    "#FFCC33",    // - Golden Yellow
    "#33CFFF",    // - Light Cyan
    "#FF3366",    // - Coral Red
    "#66FF33",    // - Lime Green
    "#FF33C4",    // - Bright Rose
    "#33FF99",    // - Spring Green
    "#FF6F33",    // - Pumpkin Orange
    "#3399FF",    // - Electric Blue
};

struct Class
{
  joda::enums::ClassId classId = joda::enums::ClassId::UNDEFINED;

  //
  //  Display name of the object class
  //
  std::string name;

  //
  //  Display color of the object class
  //
  std::string color;

  //
  //  Further notes to the object class
  //
  std::string notes;

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Class, classId, name, notes, color);
};

}    // namespace joda::settings
