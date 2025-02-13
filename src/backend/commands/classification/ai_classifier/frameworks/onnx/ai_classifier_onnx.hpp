///
/// \file      ai_lassifier_onnx.hpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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
