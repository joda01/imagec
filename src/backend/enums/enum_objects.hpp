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

struct ObjectId
{
  joda::enums::ClusterId clusterId;
  joda::enums::IteratorId iteration = {-1, -1, -1};

  bool operator<(const ObjectId &in) const
  {
    return clusterId < in.clusterId && iteration < in.iteration;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectId, clusterId, iteration);
};
}    // namespace joda::enums
