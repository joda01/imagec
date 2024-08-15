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
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::processor {
class ProcessContext;
}

namespace joda::settings {

struct ClassifierFilter
{
  struct IntensityFilter
  {
    //
    // Which image should be used for measure the intensity value.
    // If not specified the initial image of the actual pipeline step is used.
    //
    enums::ImageId imageIn;

    //
    // Min intensity
    //
    uint16_t minIntensity = 0;

    //
    // Max intensity
    //
    uint16_t maxIntensity = UINT16_MAX;

    void check() const
    {
      CHECK(maxIntensity > minIntensity, "Min intensity must be bigger than max intensity!");
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntensityFilter, imageIn, minIntensity, maxIntensity);
  };

  //
  // Cluster the objects should be assigned if filter matches
  //
  joda::enums::ClusterIdIn clusterOut = joda::enums::ClusterIdIn::$;

  //
  // Class the objects should be assigned if filter matches
  //
  joda::enums::ClassId classOut = joda::enums::ClassId::NONE;

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
  float minCircularity = 0;

  //
  //
  //
  float snapAreaSize = 0;

  //
  // Use an intensity filter for classification
  //
  std::optional<IntensityFilter> intensity;

  void check() const
  {
    CHECK(maxParticleSize < 0 || minParticleSize < 0 || maxParticleSize >= minParticleSize,
          "Max particle size must be bigger than min particle size!");
    CHECK(minCircularity >= 0 && minCircularity <= 1, "Min circularity must be in range [0-1].");
    CHECK(snapAreaSize >= 0, "Snap area size must be > 0.");
  }

  bool doesFilterMatch(joda::processor::ProcessContext &context, atom::ROI &roi,
                       const std::optional<IntensityFilter> &intensity) const;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClassifierFilter, minParticleSize, maxParticleSize,
                                                       minCircularity, snapAreaSize, intensity, clusterOut, classOut);
};

struct ObjectClass
{
  //
  // Classifies objects based on object properties
  //
  std::vector<ClassifierFilter> filters;

  //
  // If no filter matches this class is assigned to the object
  //
  joda::enums::ClusterIdIn clusterOutNoMatch = joda::enums::ClusterIdIn::$;

  //
  // If no filter matches this class is assigned to the object
  //
  joda::enums::ClassId classOutNoMatch = joda::enums::ClassId::NONE;

  //
  // Grayscale or object class id from model
  //
  int32_t modelClassId = -1;

  void check() const
  {
    CHECK(!filters.empty(), "At least one classification filter must be given!");
    CHECK(modelClassId >= 0, "A model class id >= 0 must be given for classification.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectClass, filters, clusterOutNoMatch, classOutNoMatch,
                                                       modelClassId);
};

}    // namespace joda::settings
