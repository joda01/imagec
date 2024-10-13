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
#include <vector>
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_clusters.hpp"
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

  struct IntersectionFilter
  {
    //
    // Objects to calc the intersection with
    //
    ObjectInputClusters inputClustersIntersectWith;

    //
    // Minimum intersection in [0-1]
    //
    float minIntersection = 0.1F;

    void check() const
    {
      CHECK_ERROR(!inputClustersIntersectWith.empty(), "At least one intersection class must be given!");
      CHECK_ERROR(minIntersection >= 0 && minIntersection <= 1, "Min intersection must be in range [0-1].");
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntersectionFilter, inputClustersIntersectWith, minIntersection);
  };

  //
  // What should happen when an intersection was found
  //
  Mode mode = Mode::RECLASSIFY_MOVE;

  //
  // Objects to use for intersection calculation
  //
  ObjectInputClusters inputClusters;

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
    CHECK_ERROR(!inputClusters.empty(), "At least one input must be given!");
    if(mode == Mode::RECLASSIFY_MOVE || mode == Mode::RECLASSIFY_COPY) {
      CHECK_ERROR(newClassId != joda::enums::ClassIdIn::UNDEFINED, "Define a class the elements should be assigned for reclassification.");
    }
  }

  settings::ObjectInputClusters getInputClusters() const override
  {
    settings::ObjectInputClusters clusters;
    for(const auto &in : inputClusters) {
      clusters.emplace(in);
    }

    for(const auto &in : intersection.inputClustersIntersectWith) {
      clusters.emplace(in);
    }
    return clusters;
  }

  [[nodiscard]] ObjectOutputClusters getOutputClasses() const override
  {
    ObjectOutputClusters out;
    if(mode == Mode::RECLASSIFY_MOVE || mode == Mode::RECLASSIFY_COPY) {
      for(const auto &in : intersection.inputClustersIntersectWith) {
        out.emplace(ClassificatorSetting{in.clusterId, newClassId});
      }
    }
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ReclassifySettings, mode, inputClusters, intersection, metrics, intensity, newClassId);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ReclassifySettings::Mode, {
                                                           {ReclassifySettings::Mode::RECLASSIFY_MOVE, "ReclassifyMove"},
                                                           {ReclassifySettings::Mode::RECLASSIFY_COPY, "ReclassifyCopy"},
                                                       });

}    // namespace joda::settings
