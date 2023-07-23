///
/// \file      threshold.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <iostream>
#include "image_processing/functions/detection/detection.hpp"
#include "image_processing/functions/func_types.hpp"
#include <opencv2/opencv.hpp>

namespace joda::func::threshold {

class ObjectSegmentation : public DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  ObjectSegmentation(uint16_t thresholdValue);
  auto forward(const cv::Mat &srcImg) -> DetectionResponse override;

private:
  uint16_t mThresholdValue = 0;
};

}    // namespace joda::func::threshold
