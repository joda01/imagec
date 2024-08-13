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
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ClassifierFilter
{
  int32_t minParticleSize = -1;
  int32_t maxParticleSize = -1;
  uint16_t minIntensity   = 0;
  uint16_t maxIntensity   = UINT16_MAX;
  float minCircularity    = 0;
  float snapAreaSize      = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClassifierFilter, maxParticleSize, minParticleSize, minCircularity,
                                              snapAreaSize, minIntensity, maxIntensity);
};
}    // namespace joda::settings
