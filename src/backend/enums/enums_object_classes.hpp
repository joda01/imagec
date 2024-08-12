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
/// \brief     A short description what happens here.
///

#pragma once

#include <nlohmann/json.hpp>

namespace joda::enums {

enum class ObjectClassId : int16_t
{

  NONE = -2,
  $    = -1,
  CL00 = 0,
  CL01 = 1,
  CL02 = 2,
  CL03 = 3,
  CL04 = 4,
  CL05 = 5,
  CL06 = 6,
  CL07 = 7,
  CL08 = 8,
  CL09 = 9,

};

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectClassId, {{ObjectClassId::NONE, "None"},
                                             {ObjectClassId::$, "$"},
                                             {ObjectClassId::CL00, "CL_00"},
                                             {ObjectClassId::CL01, "CL_01"},
                                             {ObjectClassId::CL02, "CL_02"},
                                             {ObjectClassId::CL03, "CL_03"},
                                             {ObjectClassId::CL04, "CL_04"},
                                             {ObjectClassId::CL05, "CL_05"},
                                             {ObjectClassId::CL06, "CL_06"},
                                             {ObjectClassId::CL07, "CL_07"},
                                             {ObjectClassId::CL08, "CL_08"},
                                             {ObjectClassId::CL09, "CL_09"}});

}    // namespace joda::enums
