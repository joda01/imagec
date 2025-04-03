
///
/// \file      enums_grouping.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
