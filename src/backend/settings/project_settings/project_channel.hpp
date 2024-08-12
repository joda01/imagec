///
/// \file      project_channels.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <set>
#include <vector>
#include <nlohmann/json.hpp>

namespace joda::settings {

struct Channel
{
  //
  //  Display name of the object class
  //
  std::string name;

  //
  //  Further notes to the object class
  //
  std::string notes;

  //
  //  Display color of the object class
  //
  std::string color;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Channel, name, notes, color);
};

}    // namespace joda::settings
