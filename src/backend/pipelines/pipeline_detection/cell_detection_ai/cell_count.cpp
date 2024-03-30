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

#include "cell_count.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string_view>
#include "backend/image_processing/detection/ai_object_segmentation/ai_object_segmentation.hpp"
#include "backend/image_processing/detection/object_segmentation/object_segmentation.hpp"

namespace joda::pipeline::detection {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
auto CellCounter::execute(const cv::Mat &img, const cv::Mat &imgOriginal,
                          const joda::settings::json::ChannelSettings &channelSetting) -> func::DetectionResponse
{
  if(channelSetting.getDetectionSettings().getDetectionMode() ==
     settings::json::ChannelDetection::DetectionMode::THRESHOLD) {
    joda::func::threshold::ObjectSegmentation th(
        channelSetting.getFilter(), channelSetting.getDetectionSettings().getThersholdSettings().getThresholdMin(),
        channelSetting.getDetectionSettings().getThersholdSettings().getThreshold(),
        channelSetting.getDetectionSettings().doWatershedSegmentation());
    return th.forward(img, imgOriginal, channelSetting.getChannelInfo().getChannelIndex());
  } else {
    joda::func::ai::ObjectSegmentation obj(&channelSetting.getFilter(),
                                           "models/cell_segmentation_brightfield_in_vitro_v1.onnx", {"cell"});
    return obj.forward(img, imgOriginal, channelSetting.getChannelInfo().getChannelIndex());
  }
}

}    // namespace joda::pipeline::detection
