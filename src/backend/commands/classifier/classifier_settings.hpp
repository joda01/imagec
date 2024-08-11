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
/// \brief     A short description what happens here.
///

#pragma once

#include <cstdint>
#include <set>
#include "backend/commands/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::cmd::functions {

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
    // Class id to identify the object with
    //
    joda::enums::ObjectClassId classId;

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

}    // namespace joda::cmd::functions
