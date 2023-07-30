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
#include "duration_count/duration_count.h"
#include "settings/channel_settings.hpp"

namespace joda::algo {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
auto CellCounter::execute(const cv::Mat &img, const cv::Mat &imgOriginal,
                          const joda::settings::json::ChannelSettings &channelSetting) -> func::DetectionResponse
{
  auto enhancedContrast = img;
  joda::func::ai::ObjectSegmentation obj(&channelSetting.getFilter(),
                                         "imagec_models/cell_segmentation_brightfield_in_vitro_v1.onnx", {"cell"});
  return obj.forward(enhancedContrast, imgOriginal);
}

}    // namespace joda::algo
