///
/// \file      spot_detection.cpp
/// \author    Joachim Danmayr
/// \date      2023-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Pipeline which implements an AI based nucleus counter
///

#include "spot_detection.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include "duration_count/duration_count.h"
#include "image_processing/functions/func_types.hpp"
#include "image_processing/object_detection/detection.hpp"
#include <opencv2/imgproc.hpp>

namespace joda::algo {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
auto SpotDetection::execute(const cv::Mat &img, const joda::settings::json::ChannelDetection &channelSetting)
    -> func::DetectionResponse
{
  auto enhancedContrast = img;
  cv::Mat binaryImage;
  // cv::adaptiveThreshold(enhancedContrast, binaryImage, UINT16_MAX, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY,
  //                       11, 2);
  float thVal = (float) UINT16_MAX * (float) channelSetting.getThersholdSettings().getThresholdMin();
  std::cout << "TH val: " << std::to_string(thVal) << std::endl;

  // enhancedContrast = UINT16_MAX - enhancedContrast;
  cv::threshold(enhancedContrast, binaryImage, thVal, UINT16_MAX, cv::THRESH_BINARY);

  return func::DetectionResponse{.result = {}, .controlImage = binaryImage * 0.003906250};
}
}    // namespace joda::algo
