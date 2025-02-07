///
/// \file      ai_model.hpp
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

#undef slots

#include <torch/torch.h>
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#define slots Q_SLOTS

namespace joda::ai {

class AiModel
{
public:
  struct Result
  {
    cv::Rect boundingBox;
    cv::Mat mask;
    std::vector<cv::Point> contour;
    int32_t classId;
    float probability;
  };

  virtual auto processPrediction(const cv::Mat &inputImage, const torch::Tensor &prediction) -> std::vector<Result> = 0;
};

}    // namespace joda::ai
