///
/// \file      project_object_class.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///

#pragma once

#include <set>
#include <vector>
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

static inline const std::vector<std::string> COLORS = {

    "#9933FF",    // - Purple

    // Blue tones
    "#3399FF",    // - Electric Blue
    "#33A1FF",    // - Sky Blue
    "#33CFFF",    // - Light Cyan
    "#33FFD1",    // - Aqua

    // Green tones
    "#33FFCC",    // - Mint Green
    "#33FF99",    // - Spring Green
    "#33FF88",    // - Neon Green
    "#33FF57",    // - Bright Green
    "#66FF33",    // - Lime Green

    // Yellow tones
    "#FFFF33",    // - Bright Yellow
    "#FFCC33",    // - Golden Yellow

    // Orange tones
    "#FF8C33",    // - Deep Orange
    "#FF6F33",    // - Pumpkin Orange
    "#FF5733",    // - Vibrant Orange
    "#FF33FF",    // - Magenta
    "#FF33C4",    // - Bright Rose
    "#FF33A1",    // - Hot Pink
    "#FF3380",    // - Electric Pink
    "#FF3333",    // - Neon red
    "#FF3366",    // - Coral Red

    // Gray
    "#BFBFBF"    // -Light gray
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
