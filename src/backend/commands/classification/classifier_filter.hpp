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
#include <optional>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/json_optional_parser_helper.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json.hpp>

namespace joda::processor {
class ProcessContext;
}

namespace joda::settings {

struct IntensityFilter
{
  //
  // Which image should be used for measure the intensity value.
  // If not specified the initial image of the actual pipeline step is used.
  //
  enums::ImageId imageIn = {.zProjection = joda::enums::ZProjection::$, .imagePlane = {.cStack = -1}};

  //
  // Min intensity
  //
  int32_t minIntensity = -1;

  //
  // Max intensity
  //
  int32_t maxIntensity = -1;

  void check() const
  {
    if(minIntensity >= 0 || maxIntensity >= 0) {
      CHECK_ERROR(maxIntensity > minIntensity, "Min intensity must be bigger than max intensity!");
    }
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntensityFilter, imageIn, minIntensity, maxIntensity);
};

struct MetricsFilter
{
  //
  //
  //
  int32_t minParticleSize = -1;

  //
  //
  //
  int32_t maxParticleSize = -1;

  //
  //
  //
  float minCircularity = -1;

  void check() const
  {
    CHECK_ERROR(maxParticleSize < 0 || minParticleSize < 0 || maxParticleSize >= minParticleSize,
                "Max particle size must be bigger than min particle size!");
    CHECK_ERROR(minCircularity < 0 || (minCircularity >= 0 && minCircularity <= 1), "Min circularity must be in range [0-1].");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(MetricsFilter, minParticleSize, maxParticleSize, minCircularity);
};

struct ClassifierFilter
{
  //
  // Classs the objects should be assigned if filter matches
  //
  enums::ClassIdIn outputClass = enums::ClassIdIn::$;

  //
  // Use an intensity filter for classification
  //
  IntensityFilter intensity;

  //
  // Metrics filter
  //
  MetricsFilter metrics;

  void check() const
  {
  }

  static bool doesFilterMatch(joda::processor::ProcessContext &context, atom::ROI &roi, const MetricsFilter &metrics,
                              const IntensityFilter &intensity);

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClassifierFilter, metrics, intensity, outputClass);
};

struct ObjectClass
{
  //
  // Classifies objects based on object properties
  //
  std::list<ClassifierFilter> filters = {{}};

  //
  // If no filter matches this class is assigned to the object
  //
  enums::ClassIdIn outputClassNoMatch = enums::ClassIdIn::NONE;

  //
  // Grayscale or object class id from model
  //
  int32_t modelClassId = -1;

  //
  // The detected probability of the selected model class is multiplied by this factor.
  // If not given the factor is 1. This can be useful to compensate bad training data for
  // some of the classes.
  //
  float probabilityHandicap = 1.0;

  void check() const
  {
    CHECK_ERROR(!filters.empty(), "At least one classification filter must be given!");
    CHECK_ERROR(modelClassId >= 0, "A model class id >= 0 must be given for classification.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectClass, filters, outputClassNoMatch, modelClassId, probabilityHandicap);
};

}    // namespace joda::settings
