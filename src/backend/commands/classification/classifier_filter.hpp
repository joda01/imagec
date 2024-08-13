///
/// \file      classifier_filter.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <cstdint>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/helper/json_optional_parser_helper.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ClassifierFilter
{
  struct IntensityFilter
  {
    //
    // Which image should be used for measure the intensity value.
    // If not specified the initial image of the actual pipeline step is used.
    //
    enums::ImageId imageId;

    //
    // Min intensity
    //
    uint16_t minIntensity = 0;

    //
    // Max intensity
    //
    uint16_t maxIntensity = UINT16_MAX;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(IntensityFilter, minIntensity, maxIntensity);
  };

  //
  // Cluster the objects should be assigned if filter matches
  //
  joda::enums::ClusterId clusterId = joda::enums::ClusterId::NONE;

  //
  // Class the objects should be assigned if filter matches
  //
  joda::enums::ClassId classId = joda::enums::ClassId::NONE;

  int32_t minParticleSize = -1;
  int32_t maxParticleSize = -1;
  float minCircularity    = 0;
  float snapAreaSize      = 0;

  //
  // Use an intensity filter for classification
  //
  std::optional<IntensityFilter> intensity;

  bool doesFilterMatch(atom::ROI &roi, const joda::atom::Image &image) const
  {
    if(intensity.has_value()) {
      auto intensity = roi.measureIntensityAndAdd(image);
      if(intensity.intensity < intensity.intensityMin || intensity.intensity > intensity.intensityMax) {
        // Intensity filter does not match
        return false;
      }
    }
    if((minParticleSize < 0 || roi.getAreaSize() >= minParticleSize) &&
       (maxParticleSize < 0 || roi.getAreaSize() <= maxParticleSize) && roi.getCircularity() >= minCircularity) {
      return true;
    }
    return false;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClassifierFilter, minParticleSize, maxParticleSize, minCircularity,
                                              snapAreaSize, intensity, clusterId, classId);
};

struct ObjectClass
{
  //
  // Classification filters
  //
  std::vector<ClassifierFilter> filters;

  //
  // If no filter matches this class is assigned to the object
  //
  joda::enums::ClusterId noMatchingClusterId = joda::enums::ClusterId::$;

  //
  // If no filter matches this class is assigned to the object
  //
  joda::enums::ClassId noMatchingClassId = joda::enums::ClassId::NONE;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectClass, filters, noMatchingClassId);
};

}    // namespace joda::settings
