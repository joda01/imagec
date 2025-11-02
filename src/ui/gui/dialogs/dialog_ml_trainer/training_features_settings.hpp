
///
/// \file      training_features_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-11-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include <set>

namespace joda::ui::gui {

struct TrainingFeaturesSettings
{
  std::set<int32_t> blurKernelSizeVariation              = {5};
  std::set<int32_t> weightedDeviationKernelSizeVariation = {5};
  std::set<int32_t> gradientMagnitudeKernelSizeVariation = {5};
};

}    // namespace joda::ui::gui
