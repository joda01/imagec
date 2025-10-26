///
/// \file      random_forest_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>

namespace joda::ml {

struct RandomForestTrainingSettings
{
  int32_t maxTreeDepth      = 50;    // 100
  int32_t minSampleCount    = 10;
  float regressionAccuracy  = 0;
  double terminationEpsilon = 0;
  int32_t maxNumberOfTrees  = 50;
};

}    // namespace joda::ml
