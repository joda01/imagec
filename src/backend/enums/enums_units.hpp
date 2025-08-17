
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

enum class Units
{
  Automatic,
  Pixels,
  nm,
  um,
  mm,
  cm,
  m,
  km
};

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(Units, {
                                        {Units::Automatic, "Automatic"},
                                        {Units::Pixels, "Px"},
                                        {Units::nm, "nm"},
                                        {Units::um, "um"},
                                        {Units::mm, "mm"},
                                        {Units::cm, "cm"},
                                        {Units::m, "m"},
                                        {Units::km, "km"},
                                    });

}    // namespace joda::enums
