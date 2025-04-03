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
#include <vector>
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/enums/enum_objects.hpp"

#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ReclassifySettings : public SettingBase
{
  enum class Mode
  {
    UNKNOWN,
    RECLASSIFY_MOVE,
    RECLASSIFY_COPY,
  };

  enum class HierarchyHandling
  {
    CREATE_TREE,      // If intersecting the intersecting object gets the object it intersects with as parent
    KEEP_EXISTING,    // The object keeps its old hierarchy information.
    REMOVE            // The hierarchy information of the object is removed.
  };

  struct IntersectionFilter
  {
    //
    // Objects to calc the intersection with
    //
    ObjectInputClasses inputClassesIntersectWith;

    //
    // Minimum intersection in [0-1]
    //
    float minIntersection = 0.1F;

    void check() const
    {
      /// \todo check that ouput is not equal to input
      CHECK_ERROR(!inputClassesIntersectWith.empty(), "At least one intersection class must be given!");
      CHECK_ERROR(minIntersection >= 0 && minIntersection <= 1, "Min intersection must be in range [0-1].");
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntersectionFilter, inputClassesIntersectWith, minIntersection);
  };

  //
  // What should happen when an intersection was found
  //
  Mode mode = Mode::RECLASSIFY_MOVE;

  //
  //
  //
  HierarchyHandling hierarchyMode = HierarchyHandling::CREATE_TREE;

  //
  // Objects to use for intersection calculation
  //
  ObjectInputClasses inputClasses;

  //
  // In case of reclassification this is the new class ID for intersecting elements
  //
  joda::enums::ClassIdIn newClassId = joda::enums::ClassIdIn::UNDEFINED;

  //
  // Apply filter if object is intersecting
  //
  IntersectionFilter intersection;

  //
  // Metrics filter
  //
  MetricsFilter metrics;

  //
  // Intensity filter
  //
  IntensityFilter intensity;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(mode != Mode::UNKNOWN, "Define a intersection function!");
    CHECK_ERROR(!inputClasses.empty(), "At least one input must be given!");
    if(mode == Mode::RECLASSIFY_MOVE || mode == Mode::RECLASSIFY_COPY) {
      CHECK_ERROR(newClassId != joda::enums::ClassIdIn::UNDEFINED, "Define a class the elements should be assigned for reclassification.");
    }
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    for(const auto &in : inputClasses) {
      classes.emplace(in);
    }

    for(const auto &in : intersection.inputClassesIntersectWith) {
      classes.emplace(in);
    }
    return classes;
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    ObjectOutputClasses out;
    out.emplace(newClassId);
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ReclassifySettings, mode, hierarchyMode, inputClasses, intersection, metrics, intensity,
                                                       newClassId);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ReclassifySettings::Mode, {
                                                           {ReclassifySettings::Mode::RECLASSIFY_MOVE, "ReclassifyMove"},
                                                           {ReclassifySettings::Mode::RECLASSIFY_COPY, "ReclassifyCopy"},
                                                       });

NLOHMANN_JSON_SERIALIZE_ENUM(ReclassifySettings::HierarchyHandling, {
                                                                        {ReclassifySettings::HierarchyHandling::CREATE_TREE, "CreateTree"},
                                                                        {ReclassifySettings::HierarchyHandling::KEEP_EXISTING, "KeepExisting"},
                                                                        {ReclassifySettings::HierarchyHandling::REMOVE, "Remove"},
                                                                    });

}    // namespace joda::settings
