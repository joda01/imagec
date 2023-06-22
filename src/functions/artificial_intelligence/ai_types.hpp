///
/// \file      ai_types.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

namespace joda::func::ai {

using Boxes      = cv::Rect;
using Confidence = float;
using ClassId    = int;

struct Detection
{
  uint32_t index;           ///< Index in the prediction array
  Confidence confidence;    ///< Probability
  ClassId classId;          ///< Class id
  Boxes box;                ///< Rectangle around the prediction
  cv::Mat boxMask;          ///< Segmentation mask
};

using DetectionResults = std::vector<Detection>;
}    // namespace joda::func::ai
