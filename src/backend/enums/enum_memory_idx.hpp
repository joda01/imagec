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

#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

enum class MemoryIdx : uint16_t
{
  M0 = 0,
  M1 = 1,
  M2 = 2,
  M3 = 3,
  M4 = 4,
  M5 = 5,
  M6 = 6,
  M7 = 7,
  M8 = 8,
  M9 = 9,
};

NLOHMANN_JSON_SERIALIZE_ENUM(MemoryIdx, {
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
                                        });

}    // namespace joda::enums
