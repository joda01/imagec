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
#include <list>
#include <set>
#include <vector>
#include "backend/enums/enum_objects.hpp"

#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json.hpp>

// #warning "Allow multi intersection"

namespace joda::settings {

struct ColocalizationSettings : public SettingBase
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

  struct InputOutputClasses
  {
    //
    // This is the origin element
    //
    enums::ClassIdIn inputClassId = enums::ClassIdIn::UNDEFINED;
    //
    // In case of reclassification this is the new class ID for intersecting elements
    //
    enums::ClassIdIn newClassId = enums::ClassIdIn::UNDEFINED;

    void check() const
    {
      if(inputClassId != enums::ClassIdIn::UNDEFINED) {
        CHECK_ERROR(newClassId != enums::ClassIdIn::UNDEFINED, "New coloc class must not be undefined!");
      }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_EXTENDED(InputOutputClasses, inputClassId, newClassId);
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
  // Classes to calculate the intersection with
  //
  std::vector<InputOutputClasses> inputClasses;

  //
  // Minimum intersection in [0-1]
  //
  float minIntersection = 0.1F;

  //
  // Resulting object classs of the intersecting objects
  //
  enums::ClassIdIn outputClass = enums::ClassIdIn::$;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(inputClasses.size() > 1, "At least two input objects must be given!");
    CHECK_ERROR(minIntersection >= 0 && minIntersection <= 1, "Min intersection must be between [0-1].");
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    for(const auto classs : inputClasses) {
      if(classs.inputClassId != enums::ClassIdIn::UNDEFINED) {
        classes.emplace(classs.inputClassId);
      }
    }

    return classes;
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    settings::ObjectInputClasses classes = {outputClass};

    for(const auto classs : inputClasses) {
      if(classs.inputClassId != enums::ClassIdIn::UNDEFINED) {
        classes.emplace(classs.newClassId);
      }
    }
    return classes;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_EXTENDED(ColocalizationSettings, mode, hierarchyMode, inputClasses, minIntersection, outputClass);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ColocalizationSettings::Mode, {
                                                               {ColocalizationSettings::Mode::RECLASSIFY_MOVE, "ReclassifyMove"},
                                                               {ColocalizationSettings::Mode::RECLASSIFY_COPY, "ReclassifyCopy"},
                                                           });

NLOHMANN_JSON_SERIALIZE_ENUM(ColocalizationSettings::HierarchyHandling,
                             {
                                 {ColocalizationSettings::HierarchyHandling::CREATE_TREE, "CreateTree"},
                                 {ColocalizationSettings::HierarchyHandling::KEEP_EXISTING, "KeepExisting"},
                                 {ColocalizationSettings::HierarchyHandling::REMOVE, "Remove"},
                             });

}    // namespace joda::settings
