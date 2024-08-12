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

enum class Slot : int16_t
{
  $   = -1,
  $1  = 1,
  $2  = 2,
  $3  = 3,
  $4  = 4,
  $5  = 5,
  $6  = 6,
  $7  = 7,
  $8  = 8,
  $9  = 9,
  $10 = 10,
  $11 = 11,
  $12 = 12,
  $13 = 13,
  $14 = 14,
  $15 = 15,
  $16 = 16
};

NLOHMANN_JSON_SERIALIZE_ENUM(Slot, {{Slot::$, "$"},
                                    {Slot::$1, "$1"},
                                    {Slot::$2, "$2"},
                                    {Slot::$3, "$3"},
                                    {Slot::$4, "$4"},
                                    {Slot::$5, "$5"},
                                    {Slot::$6, "$6"},
                                    {Slot::$7, "$7"},
                                    {Slot::$8, "$8"},
                                    {Slot::$9, "$9"},
                                    {Slot::$10, "$10"},
                                    {Slot::$11, "$11"},
                                    {Slot::$12, "$12"},
                                    {Slot::$13, "$13"},
                                    {Slot::$14, "$14"},
                                    {Slot::$15, "$15"},
                                    {Slot::$16, "$16"}});

}    // namespace joda::enums
