///
/// \file      random_forest.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "../machine_learning.hpp"
#include "backend/artifacts/object_list/object_list.hpp"
#include <opencv2/core/mat.hpp>
#include "random_forest_settings.hpp"

namespace joda::ml {

class RandomForest : public MachineLearning
{
public:
  /////////////////////////////////////////////////////
  RandomForest(const RandomForestTrainingSettings &settings) : mSettings(settings)
  {
  }

protected:
  /////////////////////////////////////////////////////
  RandomForestTrainingSettings mSettings;
};

}    // namespace joda::ml
