///
/// \file      histogram_filter.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Does image preprocessing, object detection and filtering
///

#pragma once

#include <cstdint>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/settings/channel/channel_settings.hpp"

namespace joda::pipeline {

///
/// \brief If the min threshold is lower than the value at the maximum
///        of the histogram multiplied with this factor the filter
///        will be applied.
///        Example: Min Threshold = 50
///                 Hist. Max is at 40
///                 histMinThresholdFilterFactor = 1.3  --> HistogramFilterThreshold = 65 --> Filter will be applied
///
///        Example: Min Threshold = 50
///                 Hist. Max is at 10
///                 histMinThresholdFilterFactor = 1.3  --> HistogramFilterThreshold = 13 --> Filter will not be applied
/// \author Joachim Danmayr
///
inline void applyHistogramFilter(const cv::Mat &originalImg, image::detect::DetectionResponse &detectionResult,
                                 const joda::settings::ChannelSettings &channelSetting)
{
  if(channelSetting.imageFilter.histMinThresholdFilterFactor > 0) {
    int histSize           = UINT16_MAX + 1;         // Number of bins
    float range[]          = {0, UINT16_MAX + 1};    // Pixel value range
    const float *histRange = {range};
    cv::Mat histogram;
    cv::calcHist(&originalImg, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);

    double maxVal = 0;
    int maxIdx    = -1;
    cv::minMaxIdx(histogram, NULL, &maxVal, NULL, &maxIdx);

    float filterThreshold = static_cast<float>(maxIdx) * channelSetting.imageFilter.histMinThresholdFilterFactor;
    if(channelSetting.detection.detectionMode == joda::settings::DetectionSettings::DetectionMode::THRESHOLD) {
      if(channelSetting.detection.threshold.thresholdMin < filterThreshold) {
        detectionResult.responseValidity = image::detect::ResponseDataValidity::POSSIBLE_WRONG_THRESHOLD;
      }
      std::cout << "Hist idx: " << std::to_string(maxIdx) << " | " << std::to_string(filterThreshold) << std::endl;
    }
  }
}

}    // namespace joda::pipeline
