///
/// \file      enum_cache.hpp
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

enum class MemoryId : uint16_t
{
  M0   = 0,
  M1   = 1,
  M2   = 2,
  M3   = 3,
  M4   = 4,
  M5   = 5,
  M6   = 6,
  M7   = 7,
  M8   = 8,
  M9   = 9,
  NONE = 0xFFFE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(MemoryId, {{MemoryId::NONE, "None"},
                                        {MemoryId::M0, "M0"},
                                        {MemoryId::M1, "M1"},
                                        {MemoryId::M2, "M2"},
                                        {MemoryId::M3, "M3"},
                                        {MemoryId::M4, "M4"},
                                        {MemoryId::M5, "M5"},
                                        {MemoryId::M6, "M6"},
                                        {MemoryId::M7, "M7"},
                                        {MemoryId::M8, "M8"},
                                        {MemoryId::M9, "M9"}});

}    // namespace joda::enums
