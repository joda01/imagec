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
#include "backend/image_processing/detection/object_segmentation/object_segmentation.hpp"
#include <opencv2/imgproc.hpp>

namespace joda::pipeline::detection {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
auto SpotDetection::execute(const cv::Mat &img, const cv::Mat &imgOriginal,
                            const joda::settings::json::ChannelSettings &channelSetting) -> func::DetectionResponse
{
  joda::func::threshold::ObjectSegmentation th(
      channelSetting.getFilter(), channelSetting.getDetectionSettings().getThersholdSettings().getThresholdMin(),
      channelSetting.getDetectionSettings().getThersholdSettings().getThreshold());
  return th.forward(img, imgOriginal);
}
}    // namespace joda::pipeline::detection
