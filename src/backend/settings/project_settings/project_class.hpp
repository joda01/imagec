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
