///
/// \file      measure_settings.hpp
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

#include <cstdint>
#include <list>
#include <set>
#include <vector>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct MeasureSettings
{
  //
  // Optional input object for which a measurement should be applied
  //
  joda::enums::ObjectStoreId objectIn;

  //
  // Clusters to calculate to measure for
  //
  std::set<joda::enums::ClusterIdIn> clustersIn = {joda::enums::ClusterIdIn::$};

  //
  // Classes to calculate to measure for
  //
  std::set<joda::enums::ClassId> classesIn;

  //
  // Image planes on which a measurement should be applied
  //
  std::list<enums::ImageId> planesIn;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(!planesIn.empty(), "At least one image plane must be given for measurement.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(MeasureSettings, objectIn, clustersIn, classesIn, planesIn);
};

}    // namespace joda::settings
