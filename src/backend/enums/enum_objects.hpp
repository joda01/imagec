///
/// \file      enum_objects.hpp
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
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

struct ObjectStoreId
{
  joda::enums::MemoryIdxIn storeIdx = joda::enums::MemoryIdxIn::$;
  joda::enums::PlaneId imagePlane;

  bool operator<(const ObjectStoreId &in) const
  {
    return storeIdx < in.storeIdx && imagePlane < in.imagePlane;
  }

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectStoreId, storeIdx, imagePlane);
};

}    // namespace joda::enums