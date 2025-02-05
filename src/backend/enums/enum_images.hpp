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

#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/types.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

struct ImageId
{
  //
  // If not using memory but loading image directly, the z-projection mode used for image loasing
  //
  ZProjection zProjection = ZProjection::$;

  //
  // Image plane to load
  //
  joda::enums::PlaneId imagePlane;

  //
  // If a memory ID is given the image is loaded from the memory instead of using the given plane
  //
  MemoryIdx::Enum memoryId = MemoryIdx::NONE;

  bool operator<(const ImageId &in) const
  {
    if(memoryId == MemoryIdx::NONE) {
      stdi::uint128_t plane1 = (imagePlane.toInt(imagePlane) << 8) | static_cast<uint8_t>(zProjection);
      stdi::uint128_t plane2 = (in.imagePlane.toInt(in.imagePlane) << 8) | static_cast<uint8_t>(in.zProjection);

      return plane1 < plane2;
    }
    return memoryId < in.memoryId;
  }

  void check() const
  {
    CHECK_ERROR(zProjection != enums::ZProjection::UNDEFINED, "Define the z-projection mode for image loading!");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageId, zProjection, imagePlane, memoryId);
};

}    // namespace joda::enums
