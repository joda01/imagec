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
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ClassifierSettings : public Setting
{
  struct ObjectClass
  {
    //
    // Cluster the objects should be assigned to
    //
    joda::enums::ClusterId clusterId;

    //
    // Class id to identify the object with based on the filter
    // If no filter matches the NONE class is applied.
    //
    std::map<joda::enums::ClassId, ClassifierFilter> classes;

    //
    // The grayscale value which is associated to the class
    //
    uint16_t grayscaleValue = UINT16_MAX;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectClass, clusterId, classes, grayscaleValue);
  };

  std::vector<ObjectClass> objectClasses;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClassifierSettings, objectClasses);
};

}    // namespace joda::settings
