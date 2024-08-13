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

enum class ClusterId : uint16_t
{
  A         = 0,
  B         = 1,
  C         = 2,
  D         = 3,
  E         = 4,
  F         = 5,
  G         = 6,
  H         = 7,
  I         = 8,
  J         = 9,
  UNDEFINED = 0xFFFD,
  NONE      = 0xFFFE,
  $         = 0xFFFF
};

inline int32_t toInt(ClusterId idx)
{
  return static_cast<int32_t>(idx);
}

NLOHMANN_JSON_SERIALIZE_ENUM(ClusterId, {
                                            {ClusterId::NONE, "None"},
                                            {ClusterId::UNDEFINED, "Undefined"},
                                            {ClusterId::$, "$"},
                                            {ClusterId::A, "A"},
                                            {ClusterId::B, "B"},
                                            {ClusterId::C, "C"},
                                            {ClusterId::D, "D"},
                                            {ClusterId::E, "E"},
                                            {ClusterId::F, "F"},
                                            {ClusterId::G, "G"},
                                            {ClusterId::H, "H"},
                                            {ClusterId::I, "I"},
                                            {ClusterId::J, "J"},
                                        });

}    // namespace joda::enums
