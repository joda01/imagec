///
/// \file      pipeline_indexes.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
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

enum class ClassId : uint16_t
{

  Cl0  = 0,
  Cl1  = 1,
  Cl2  = 2,
  Cl3  = 3,
  Cl4  = 4,
  Cl5  = 5,
  Cl6  = 6,
  Cl7  = 7,
  Cl8  = 8,
  Cl9  = 9,
  NONE = 0xFFFE,
  $    = 0xFFFF

};

NLOHMANN_JSON_SERIALIZE_ENUM(ClassId, {{ClassId::NONE, "None"},
                                       {ClassId::$, "$"},
                                       {ClassId::Cl0, "Cl0"},
                                       {ClassId::Cl1, "Cl1"},
                                       {ClassId::Cl2, "Cl2"},
                                       {ClassId::Cl3, "Cl3"},
                                       {ClassId::Cl4, "Cl4"},
                                       {ClassId::Cl5, "Cl5"},
                                       {ClassId::Cl6, "Cl6"},
                                       {ClassId::Cl7, "Cl7"},
                                       {ClassId::Cl8, "Cl8"},
                                       {ClassId::Cl9, "Cl9"}});

}    // namespace joda::enums
