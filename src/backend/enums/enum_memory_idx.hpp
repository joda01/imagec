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
#include "backend/enums/types.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

class MemoryIdx
{
public:
  enum Enum
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
    NONE        = 0xFE,
    $           = 0xFF,
    RESERVED_01 = 0x100,
  };

  MemoryIdx(const MemoryIdx &inData) : value(inData.value)
  {
  }

  MemoryIdx(const MemoryIdx &&inData) : value(inData.value)
  {
  }

  MemoryIdx(Enum idx) : value(0, static_cast<uint64_t>(idx))
  {
  }

  // user-defined copy assignment (copy-and-swap idiom)
  MemoryIdx &operator=(MemoryIdx other)
  {
    std::swap(value, other.value);
    return *this;
  }

  MemoryIdx(const joda::enums::PlaneId &plane, const enums::ZProjection &zProject, Enum idx) :
      value(((static_cast<uint64_t>(plane.cStack) << 32) | plane.zStack),
            ((static_cast<uint64_t>(plane.tStack) << 32) | ((static_cast<uint32_t>(zProject)) + static_cast<uint32_t>(idx))))
  {
  }

  constexpr bool operator==(const MemoryIdx &other) const
  {
    return value == other.value;
  }

  bool operator<(const MemoryIdx &other) const
  {
    return value < other.value;
  }

  bool operator==(const Enum &other) const
  {
    return stdi::uint128_t(0, other) == value;
  }

  explicit operator stdi::uint128_t() const
  {
    return value;
  }

private:
  stdi::uint128_t value;
};

NLOHMANN_JSON_SERIALIZE_ENUM(MemoryIdx::Enum, {
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

enum class MemoryScope
{
  PIPELINE,
  ITERATION
};

NLOHMANN_JSON_SERIALIZE_ENUM(MemoryScope, {
                                              {MemoryScope::PIPELINE, "Pipeline"},
                                              {MemoryScope::ITERATION, "Iteration"},
                                          });

}    // namespace joda::enums
