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

#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

using tile_t   = std::tuple<int32_t, int32_t>;
using zStack_t = int32_t;
using tStack_t = int32_t;
using cStack_t = int32_t;

struct PlaneId
{
  tStack_t tStack = -1;
  zStack_t zStack = -1;
  cStack_t cStack = -1;

  bool operator<(const PlaneId &in) const
  {
    return tStack < in.tStack || zStack < in.zStack || cStack < in.cStack;
  }

  void check()
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(PlaneId, tStack, zStack, cStack);
};

}    // namespace joda::enums
