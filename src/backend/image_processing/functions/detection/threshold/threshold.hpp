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
#include "../../../functions/detection/detection.hpp"
#include "../../../functions/func_types.hpp"
#include <opencv2/opencv.hpp>

namespace joda::func::threshold {

class ObjectSegmentation : public DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  ObjectSegmentation(const joda::settings::json::ChannelFiltering &filt, uint16_t thresholdValue);
  auto forward(const cv::Mat &srcImg, const cv::Mat &originalImage) -> DetectionResponse override;

private:
  uint16_t mThresholdValue = 0;
};

}    // namespace joda::func::threshold