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

struct AnnMlpTrainingSettings
{
  std::vector<int32_t> neuronsLayer = {64, 32};    // Vector size is nr. of neuron layers. Entry is the number of neurons per layer.
  double terminationEpsilon         = 0.05;
  int32_t maxIterations             = 1000000;
  int32_t batchSize                 = 0;
  double learningRate               = 0.001;
};

}    // namespace joda::ml
