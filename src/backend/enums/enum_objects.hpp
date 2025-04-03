///
/// \file      enum_objects.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

struct ObjectStoreId
{
  joda::enums::MemoryIdx::Enum storeIdx = joda::enums::MemoryIdx::M0;
  joda::enums::PlaneId imagePlane;

  bool operator<(const ObjectStoreId &in) const
  {
    stdi::uint128_t plane1 = (imagePlane.toInt(imagePlane) << 8) | static_cast<uint8_t>(storeIdx);
    stdi::uint128_t plane2 = (in.imagePlane.toInt(in.imagePlane) << 8) | static_cast<uint8_t>(in.storeIdx);

    return plane1 < plane2;
  }

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectStoreId, storeIdx, imagePlane);
};

}    // namespace joda::enums
