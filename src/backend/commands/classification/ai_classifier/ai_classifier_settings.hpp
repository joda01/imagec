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

struct AiClassifierSettings
{
  //
  // Path to the AI model which should be used for classification
  //
  std::string modelPath;

  //
  // Number of classes the AI model was trained with
  //
  int32_t numberOfModelClasses = 0;

  //
  // Default class threshold used to mark an object as object
  //
  float classThreshold = 0.5;

  //
  // Vector array index is the class ID used by the AI model starting with 0
  //
  std::vector<ObjectClass> classifiers;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(!modelPath.empty(), "A AI model path must be given!");
    CHECK(std::filesystem::exists(modelPath), "AI model >" + modelPath + "< cannot be opened!");
    CHECK(classThreshold >= 0, "Class threshold must be >0.");
    CHECK(numberOfModelClasses > 0, "Number of model classes be >0.");
    CHECK(!classifiers.empty(), "At least one classifier must be given!");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(AiClassifierSettings, modelPath, classThreshold,
                                                       numberOfModelClasses, classifiers);
};

}    // namespace joda::settings