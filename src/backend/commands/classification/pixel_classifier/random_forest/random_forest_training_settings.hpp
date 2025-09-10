///
/// \file      random_forest_training_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-09-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"

namespace joda::settings {

struct RandomForestTrainingSettings
{
  int32_t maxTreeDepth     = 0;
  int32_t minSampleCount   = 10;
  float regressionAccuracy = 0;
  float terminationEpsilon = 0;
  int32_t maxNumberOfTrees = 50;
};

}    // namespace joda::settings
