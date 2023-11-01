///
/// \file      nucleus_count.cpp
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

#include "nucleus_count.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string_view>
#include "backend/image_processing/detection/ai_object_detection/ai_object_detection.h"
#include "backend/image_processing/detection/threshold/threshold.hpp"

namespace joda::pipeline::detection {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
auto NucleusCounter::execute(const cv::Mat &img, const cv::Mat &imgOriginal,
                             const joda::settings::json::ChannelSettings &channelSetting) -> func::DetectionResponse
{
  auto enhancedContrast = img;

  if(channelSetting.getDetectionSettings().getDetectionMode() ==
     settings::json::ChannelDetection::DetectionMode::THRESHOLD) {
    joda::func::threshold::ObjectSegmentation th(
        channelSetting.getFilter(), channelSetting.getDetectionSettings().getThersholdSettings().getThresholdMin());
    return th.forward(img, imgOriginal);
  } else {
    joda::func::ai::ObjectDetector obj(&channelSetting.getFilter(), "imagec_models/nucleus_detection_ex_vivo_v1.onnx",
                                       {"nuclues", "nucleus_no_focus"});
    return obj.forward(enhancedContrast, imgOriginal);
  }
}
}    // namespace joda::pipeline::detection
