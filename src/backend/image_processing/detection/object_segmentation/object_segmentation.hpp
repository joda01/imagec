///
/// \file      object_segmentation.hpp
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
#include <memory>
#include "../detection.hpp"
#include "../detection_response.hpp"
#include "backend/image_processing/functions/threshold/threshold.hpp"
#include "backend/settings/channel_settings.hpp"
#include <opencv2/opencv.hpp>

namespace joda::func::threshold {

class ObjectSegmentation : public DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  ObjectSegmentation(const joda::settings::json::ChannelFiltering &filt, uint16_t thresholdValue,
                     joda::settings::json::ThresholdSettings::Threshold method);
  auto forward(const cv::Mat &srcImg, const cv::Mat &originalImage, int32_t channelIndex) -> DetectionResponse override;

private:
  std::shared_ptr<img::Threshold> mThresoldMethod;
};

}    // namespace joda::func::threshold
