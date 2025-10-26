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
#include <vector>

namespace joda::ml {

struct KNearestTrainingSettings
{
  int32_t defaultK = 10;
};

}    // namespace joda::ml
