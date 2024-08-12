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
#include "backend/enums/enums_channels.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ClassifierSettings : public Setting
{
  struct ObjectClass
  {
    struct Filter
    {
      int32_t minParticleSize = -1;
      int32_t maxParticleSize = -1;
      float minCircularity    = 0;
      float snapAreaSize      = 0;

      NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Filter, maxParticleSize, minParticleSize, minCircularity,
                                                  snapAreaSize);
    };

    //
    // Filter which should be applied
    //
    Filter filter;

    //
    // Channel the objects should be assigned to
    //
    joda::enums::ChannelId channelId;

    //
    // Class id to identify the object with
    //
    joda::enums::ClassId classId;

    //
    // The grayscale value which is associated to the class
    //
    uint16_t grayscaleValue = UINT16_MAX;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectClass, classId, grayscaleValue, filter);
  };

  std::vector<ObjectClass> objectClasses;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClassifierSettings, objectClasses);
};

}    // namespace joda::settings
