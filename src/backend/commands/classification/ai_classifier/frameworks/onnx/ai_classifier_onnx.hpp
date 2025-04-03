///
/// \file      ai_lassifier_onnx.hpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <string>
#include "backend/commands/classification/ai_classifier/frameworks/ai_framework.hpp"

namespace joda::ai {

class AiFrameworkOnnx : public AiFramework
{
public:
  /////////////////////////////////////////////////////
  AiFrameworkOnnx(const std::string &modelPath, const InputParameters &inputParameters);
  auto predict(const cv::Mat &originalImage) -> at::IValue override;

private:
  /////////////////////////////////////////////////////
  const InputParameters &mSettings;
  const std::string mModelPath;
};
}    // namespace joda::ai
