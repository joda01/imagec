///
/// \file      classifier_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include <set>
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/enums/enums_classes.hpp"

#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ClassifierSettings : public SettingBase
{
  enum class HierarchyMode
  {
    OUTER,
    INNER,
    INNER_AND_OUTER
  };

  //
  // Hierarchy Mode
  //
  HierarchyMode hierarchyMode = HierarchyMode::OUTER;

  //
  // Object classification based on gray scale value (default: modelClassId = 65535)
  //
  std::list<ObjectClass> modelClasses;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(!modelClasses.empty(), "At least one classifier must be given!");
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    ObjectOutputClasses out;
    for(const auto &clas : modelClasses) {
      out.emplace(clas.outputClassNoMatch);
      for(const auto &clasInner : clas.filters) {
        out.emplace(clasInner.outputClass);
      }
    }
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClassifierSettings, hierarchyMode, modelClasses);
};

}    // namespace joda::settings
