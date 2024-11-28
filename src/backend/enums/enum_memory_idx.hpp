///
/// \file      enum_memory_idx.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/enums/enums_classes.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

enum class MemoryIdx : __uint128_t
{
  M0  = 0x0,
  M1  = 0x1,
  M2  = 0x2,
  M3  = 0x3,
  M4  = 0x4,
  M5  = 0x5,
  M6  = 0x6,
  M7  = 0x7,
  M8  = 0x8,
  M9  = 0x9,
  M10 = 0xA,
  M11 = 0xB,
  M12 = 0xC,
  M13 = 0xD,
  M14 = 0xE,
  M15 = 0xF,
  // From 0x100 all the numbers are reserved
  RESERVED_01 = 0x100,
  NONE        = (static_cast<__uint128_t>(0xFFFFFFFFFFFFFFFF) << 64) | 0xFFFFFFFFFFFFFFFE,
  $           = (static_cast<__uint128_t>(0xFFFFFFFFFFFFFFFF) << 64) | 0xFFFFFFFFFFFFFFFF
};

inline std::string uint128ToString(__uint128_t value)
{
  std::ostringstream oss;

  // Use the same logic to extract digits
  if(value == 0) {
    oss << "0";
  } else {
    while(value > 0) {
      oss.put('0' + (value % 10));
      value /= 10;
    }
  }

  // Reverse the string stream result
  std::string result = oss.str();
  std::reverse(result.begin(), result.end());

  return result;
}

NLOHMANN_JSON_SERIALIZE_ENUM(MemoryIdx, {
                                            {MemoryIdx::$, "$"},
                                            {MemoryIdx::NONE, "None"},
                                            {MemoryIdx::M0, "M0"},
                                            {MemoryIdx::M1, "M1"},
                                            {MemoryIdx::M2, "M2"},
                                            {MemoryIdx::M3, "M3"},
                                            {MemoryIdx::M4, "M4"},
                                            {MemoryIdx::M5, "M5"},
                                            {MemoryIdx::M6, "M6"},
                                            {MemoryIdx::M7, "M7"},
                                            {MemoryIdx::M8, "M8"},
                                            {MemoryIdx::M9, "M9"},
                                            {MemoryIdx::M10, "M10"},
                                            {MemoryIdx::M11, "M11"},
                                            {MemoryIdx::M12, "M12"},
                                            {MemoryIdx::M13, "M13"},
                                            {MemoryIdx::M14, "M14"},
                                            {MemoryIdx::M15, "M15"},
                                        });

}    // namespace joda::enums
