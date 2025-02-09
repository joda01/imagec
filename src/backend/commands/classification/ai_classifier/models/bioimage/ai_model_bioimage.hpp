///
/// \file      ai_model_bioimage.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/commands/classification/ai_classifier/models/ai_model.hpp"

namespace joda::ai {

class AiModelBioImage : public AiModel
{
public:
  struct ProbabilitySettings
  {
    float maskThreshold    = 0.5;
    float contourThreshold = 0.3;
  };

  AiModelBioImage(const ProbabilitySettings &settings);
  auto processPrediction(const cv::Mat &inputImage, const at::IValue &prediction) -> std::vector<Result> override;

private:
  /////////////////////////////////////////////////////
  const ProbabilitySettings &mSettings;
};

}    // namespace joda::ai
