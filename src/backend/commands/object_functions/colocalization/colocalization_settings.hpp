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

  enum class TrackingMode
  {
    KEEP_EXISTING,    // The object keeps its old linking information.
    OVERRIDE          // The object starts with new linking information
  };

  struct InputOutputClasses
  {
    //
    // This is the origin element
    //
    enums::ClassIdIn inputClassId = enums::ClassIdIn::UNDEFINED;

    //
    // This is the new class ID for intersecting elements
    //
    enums::ClassIdIn newClassId = enums::ClassIdIn::UNDEFINED;

    //
    // This is the new class ID for not intersecting elements
    //
    enums::ClassIdIn newClassIdNotIntersecting = enums::ClassIdIn::NONE;

    void check() const
    {
      if(inputClassId != enums::ClassIdIn::UNDEFINED) {
        CHECK_ERROR(newClassId != enums::ClassIdIn::UNDEFINED, "New coloc class must not be undefined!");
      }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(InputOutputClasses, inputClassId, newClassId, newClassIdNotIntersecting);
  };

  //
  // What should happen when an intersection was found
  //
  Mode mode = Mode::RECLASSIFY_COPY;

  //
  // Remove old linking information and override with new one or keep old on
  //
  TrackingMode trackingMode = TrackingMode::OVERRIDE;

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
    for(const auto &classs : inputClasses) {
      if(classs.inputClassId != enums::ClassIdIn::UNDEFINED) {
        classes.emplace(classs.inputClassId);
      }
    }

    return classes;
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    settings::ObjectInputClasses classes = {outputClass};

    for(const auto &classs : inputClasses) {
      if(classs.inputClassId != enums::ClassIdIn::UNDEFINED) {
        classes.emplace(classs.newClassId);
        classes.emplace(classs.newClassIdNotIntersecting);
      }
    }
    return classes;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ColocalizationSettings, mode, trackingMode, inputClasses, minIntersection, outputClass);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ColocalizationSettings::Mode, {
                                                               {ColocalizationSettings::Mode::RECLASSIFY_MOVE, "ReclassifyMove"},
                                                               {ColocalizationSettings::Mode::RECLASSIFY_COPY, "ReclassifyCopy"},
                                                           });

NLOHMANN_JSON_SERIALIZE_ENUM(ColocalizationSettings::TrackingMode, {{ColocalizationSettings::TrackingMode::OVERRIDE, "Override"},
                                                                    {ColocalizationSettings::TrackingMode::KEEP_EXISTING, "KeepExisting"}});

}    // namespace joda::settings
