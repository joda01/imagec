///
/// \file      classifier_settings.hpp
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
#include <set>
#include <vector>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct MeasureSettings
{
  struct Input
  {
    //
    // From which imagePlane the image should be taken (use -1 to take the stack from the actual imagePlane)
    //
    joda::enums::PlaneId iteratorId;

    //
    // Which image from this imagePlane should be taken (use I0 to take the initial unedited original image)
    //
    joda::enums::ImageId imageId = joda::enums::ImageId::I0;
  };

  struct Output
  {
    //
    // Resulting object class of the intersecting objects
    //
    joda::enums::ClassId objectClass = joda::enums::ClassId::NONE;

    //
    // Resulting object class of the intersecting objects
    //
    joda::enums::ClusterId clusterId = joda::enums::ClusterId::NONE;
  };
  int a;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(MeasureSettings, a);
};

}    // namespace joda::settings
