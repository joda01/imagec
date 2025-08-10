

///
/// \file      ai_model_unet.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include "backend/commands/classification/ai_classifier/models/ai_model.hpp"

namespace joda::ai {

class AiModelCyto3 : public AiModel
{
public:
  struct ProbabilitySettings
  {
    float maskThreshold  = 0.8;
    float classThreshold = 0.5;
  };

  AiModelCyto3(const ProbabilitySettings &settings);
  auto processPrediction(const at::Device &device, const cv::Mat &inputImage, const at::IValue &tensorIn) -> std::vector<Result> override;

private:
  /////////////////////////////////////////////////////
  const ProbabilitySettings &mSettings;
};

}    // namespace joda::ai
