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

struct AiClassifierSettings : public Setting
{
  //
  // Path to the AI model which should be used for classification
  //
  std::string modelPath;

  //
  // Default class threshold used to mark an object as object
  //
  float classThreshold = 0.5;

  //
  // Number of classes the AI model was trained with
  //
  int32_t numberOfClasses = 1;

  //
  // Vector array index is the class ID used by the AI model starting with 0
  //
  std::vector<ObjectClass> objectClasses;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AiClassifierSettings, modelPath, classThreshold, numberOfClasses,
                                              objectClasses);
};

}    // namespace joda::settings
