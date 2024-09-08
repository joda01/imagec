
///
/// \file      enums_grouping.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <nlohmann/json.hpp>

namespace joda::enums {

enum class GroupBy
{
  OFF,
  DIRECTORY,
  FILENAME
};

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(GroupBy, {
                                          {GroupBy::OFF, "Off"},
                                          {GroupBy::DIRECTORY, "Directory"},
                                          {GroupBy::FILENAME, "Filename"},
                                      });

}    // namespace joda::enums
