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

struct ClassifierSettings : public SettingBase
{
  //
  // Object classification based on gray scale value (default: modelClassId = 65535)
  //
  std::list<ObjectClass> classifiers;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(!classifiers.empty(), "At least one classifier must be given!");
  }

  [[nodiscard]] ObjectOutputClusters getOutputClasses() const override
  {
    ObjectOutputClusters out;
    for(const auto &clas : classifiers) {
      out.emplace(clas.outputClusterNoMatch);
      for(const auto &clasInner : clas.filters) {
        out.emplace(clasInner.outputCluster);
      }
    }
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClassifierSettings, classifiers);
};

}    // namespace joda::settings
