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
#include "backend/settings/channel/channel_settings_filter.hpp"
#include "backend/settings/detection/detection_settings_threshold.hpp"
#include <opencv2/opencv.hpp>

namespace joda::image::segment {

class ObjectSegmentation : public joda::image::detect::DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  ObjectSegmentation(const joda::settings::ChannelSettingsFilter &filt, uint16_t thresholdValueMin,
                     uint16_t thresholdValueMax, joda::settings::ThresholdSettings::Mode method, bool doWatershed);
  auto forward(const cv::Mat &srcImg, const cv::Mat &originalImage, joda::settings::ChannelIndex channelIndex)
      -> joda::image::detect::DetectionResponse override;

private:
  std::shared_ptr<image::func::Threshold> mThresoldMethod;
  bool mDoWatershed;
};

}    // namespace joda::image::segment