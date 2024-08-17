///
/// \file      validator_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/types.hpp"

namespace joda::settings {

struct ThresholdValidatorSettings
{
  enum class FilterMode
  {
    UNKNOWN,
    INVALIDATE_IMAGE,
    INVALIDATE_IMAGE_PLANE,
    INVALIDATE_IAMGE_PLANE_CLUSTER
  };

  //
  // On which part the filter should be applied
  //
  FilterMode mode = FilterMode::UNKNOWN;

  //
  // Image which should be used for the validation
  //
  enums::ImageId imageIn = {.imageIdx = enums::ZProjection::$};

  //
  // Cluster on which the result should be applied to
  //
  enums::ClusterIdIn clusterIn = enums::ClusterIdIn::$;

  //
  // If the min threshold is lower than the value at the maximum
  // of the histogram multiplied with this factor the filter
  //
  float histMinThresholdFilterFactor = 0;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(mode != FilterMode::UNKNOWN, "Define a filter mode!");
    CHECK(histMinThresholdFilterFactor >= 0, "Thresholdfactor must be >=0!");
    if(mode == FilterMode::INVALIDATE_IAMGE_PLANE_CLUSTER) {
      CHECK(clusterIn != enums::ClusterIdIn::NONE, "Cluster must not be >NONE<!");
    }
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ThresholdValidatorSettings, mode, imageIn, clusterIn,
                                                       histMinThresholdFilterFactor);
};

}    // namespace joda::settings
