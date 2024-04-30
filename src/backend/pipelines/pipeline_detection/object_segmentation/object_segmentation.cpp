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

#include "object_segmentation.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string_view>
#include "backend/image_processing/detection/ai_object_segmentation/ai_object_segmentation.hpp"
#include "backend/image_processing/detection/object_segmentation/object_segmentation.hpp"

namespace joda::pipeline::detection {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
auto ObjectSegmentation::execute(const cv::Mat &img, const cv::Mat &imgOriginal,
                                 const joda::settings::ChannelSettings &channelSetting) -> func::DetectionResponse
{
  if(channelSetting.detection.detectionMode == settings::DetectionSettings::DetectionMode::THRESHOLD) {
    joda::func::threshold::ObjectSegmentation th(
        channelSetting.objectFilter, channelSetting.detection.threshold.thresholdMin,
        channelSetting.detection.threshold.mode, channelSetting.detection.threshold.$watershedSegmentation.enabled);
    return th.forward(img, imgOriginal, channelSetting.meta.channelIdx);
  } else {
    auto modelData = getAvailableModels().find(channelSetting.detection.ai.modelPath);
    if(modelData != getAvailableModels().end()) {
      joda::func::ai::ObjectSegmentation obj(channelSetting.objectFilter, modelData->second,
                                             channelSetting.detection.ai.minProbability);
      return obj.forward(img, imgOriginal, channelSetting.meta.channelIdx);
    } else {
      throw std::runtime_error("Selected model >" + channelSetting.detection.ai.modelPath +
                               "< not found in model path!");
    }
  }
}

}    // namespace joda::pipeline::detection
