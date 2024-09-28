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
#include "backend/settings/setting_base.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct AiClassifierSettings : public SettingBase
{
  //
  // Path to the AI model which should be used for classification
  //
  std::string modelPath;

  //
  // Number of classes the AI model was trained with
  //
  int32_t numberOfModelClasses = 1;

  //
  // Default class threshold used to mark an object as object
  //
  float classThreshold = 0.5;

  //
  // Vector array index is the class ID used by the AI model starting with 0
  //
  std::vector<ObjectClass> modelClasses = {{}};

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(!modelPath.empty(), "A AI model path must be given!");
    CHECK_ERROR(std::filesystem::exists(modelPath), "AI model >" + modelPath + "< cannot be opened!");
    CHECK_ERROR(classThreshold >= 0, "Class threshold must be >0.");
    CHECK_ERROR(numberOfModelClasses > 0, "Number of model classes be >0.");
    CHECK_ERROR(!modelClasses.empty(), "At least one classifier must be given!");
  }

  [[nodiscard]] ObjectOutputClusters getOutputClasses() const override
  {
    ObjectOutputClusters out;
    for(const auto &clas : modelClasses) {
      out.emplace(clas.outputClusterNoMatch);
      for(const auto &clasInner : clas.filters) {
        out.emplace(clasInner.outputCluster);
      }
    }
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(AiClassifierSettings, modelPath, classThreshold, numberOfModelClasses, modelClasses);
};

}    // namespace joda::settings
