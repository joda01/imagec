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
#include "image_processing/functions/detection/threshold/threshold.hpp"
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
  joda::func::threshold::ObjectSegmentation th(channelSetting.getThersholdSettings().getThresholdMin());
  return th.forward(img);
}
}    // namespace joda::algo
