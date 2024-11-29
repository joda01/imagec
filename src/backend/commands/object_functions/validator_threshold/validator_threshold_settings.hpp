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
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/settings/setting_base.hpp"
#include "backend/settings/settings_types.hpp"

namespace joda::settings {

struct ThresholdValidatorSettings : public SettingBase
{
  enum class FilterMode
  {
    UNKNOWN,
    INVALIDATE_IMAGE,
    INVALIDATE_IMAGE_PLANE,
    INVALIDATE_IAMGE_PLANE_CLASS
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
  // Classs on which the result should be applied to
  //
  enums::ClassIdIn inputClasses = enums::ClassIdIn::$;

  //
  // If the min threshold is lower than the value at the maximum
  // of the histogram multiplied with this factor the filter
  //
  float histMinThresholdFilterFactor = 0.8;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(mode != FilterMode::UNKNOWN, "Define a filter mode!");
    CHECK_ERROR(histMinThresholdFilterFactor >= 0, "Thresholdfactor must be >=0!");
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    return {inputClasses};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ThresholdValidatorSettings, mode, imageIn, inputClasses, histMinThresholdFilterFactor);
};

}    // namespace joda::settings
