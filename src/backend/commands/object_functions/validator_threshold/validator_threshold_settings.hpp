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

#include <set>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/types.hpp"

namespace joda::settings {

struct ThresholdValidatorSettings : public SettingBase
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
  FilterMode mode = FilterMode::INVALIDATE_IMAGE;

  //
  // Image which should be used for the validation
  //
  enums::ImageId imageIn = {.zProjection = enums::ZProjection::$};

  //
  // Cluster on which the result should be applied to
  //
  enums::ClusterIdIn inputCluster = enums::ClusterIdIn::$;

  //
  // If the min threshold is lower than the value at the maximum
  // of the histogram multiplied with this factor the filter
  //
  float histMinThresholdFilterFactor = 0.8;

  /////////////////////////////////////////////////////
  void check()
  {
    CHECK_ERROR(mode != FilterMode::UNKNOWN, "Define a filter mode!");
    CHECK_ERROR(histMinThresholdFilterFactor >= 0, "Thresholdfactor must be >=0!");
  }

  std::set<enums::ClusterIdIn> getInputClusters() const override
  {
    return {inputCluster};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ThresholdValidatorSettings, mode, imageIn, inputCluster,
                                                       histMinThresholdFilterFactor);
};

}    // namespace joda::settings
