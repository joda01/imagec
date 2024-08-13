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
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

enum class ObjectStoreIdx : uint16_t
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

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectStoreIdx, {{ObjectStoreIdx::NONE, "None"},
                                              {ObjectStoreIdx::M0, "M0"},
                                              {ObjectStoreIdx::M1, "M1"},
                                              {ObjectStoreIdx::M2, "M2"},
                                              {ObjectStoreIdx::M3, "M3"},
                                              {ObjectStoreIdx::M4, "M4"},
                                              {ObjectStoreIdx::M5, "M5"},
                                              {ObjectStoreIdx::M6, "M6"},
                                              {ObjectStoreIdx::M7, "M7"},
                                              {ObjectStoreIdx::M8, "M8"},
                                              {ObjectStoreIdx::M9, "M9"}});

struct ObjectStoreId
{
  joda::enums::ObjectStoreIdx storeIdx = joda::enums::ObjectStoreIdx::NONE;
  joda::enums::IteratorId iteration    = {-1, -1, -1};

  bool operator<(const ObjectStoreId &in) const
  {
    return storeIdx < in.storeIdx && iteration < in.iteration;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectStoreId, storeIdx, iteration);
};

}    // namespace joda::enums
