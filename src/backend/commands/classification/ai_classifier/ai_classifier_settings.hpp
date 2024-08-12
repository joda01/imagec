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
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct AiClassifierSettings : public Setting
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

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectClass, classId, filter);
  };

  //
  // Path to the AI model which should be used
  //
  std::string modelPath;

  //
  //
  //
  float classThreshold = 0.5;

  //
  // Number of classes the AI model was trained with
  //
  int32_t numberOfClasses = 1;

  //
  // Key is the class ID used by the AI model
  //
  std::map<int32_t, ObjectClass> objectClasses;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AiClassifierSettings, objectClasses);
};

}    // namespace joda::settings
