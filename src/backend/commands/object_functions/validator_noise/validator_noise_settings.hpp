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

namespace joda::settings {

struct NoiseValidatorSettings : public SettingBase
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
  ObjectInputClasses inputClasses;

  //
  // If this number of objects is exceeded the filter will be applied
  //
  uint32_t maxObjects = 100000;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(mode != FilterMode::UNKNOWN, "Define a filter mode!");
    CHECK_ERROR(maxObjects > 0, "Max objects must be > 0!");
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    for(const auto &in : inputClasses) {
      classes.emplace(in);
    }
    return classes;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(NoiseValidatorSettings, mode, imageIn, inputClasses, maxObjects);
};

}    // namespace joda::settings
