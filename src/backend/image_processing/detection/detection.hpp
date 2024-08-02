///
/// \file      detection.hpp
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

#include <exception>
#include <string>
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/settings/channel/channel_settings_filter.hpp"
#include <opencv2/imgproc.hpp>
#include "detection_response.hpp"

namespace joda::image::detect {

///
/// \class      Detection
/// \author     Joachim Danmayr
/// \brief
///
class DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  DetectionFunction(const joda::settings::ChannelSettingsFilter &filt) : mFilterSettings(filt)
  {
  }

  /////////////////////////////////////////////////////
  virtual auto forward(const cv::Mat &srcImg, const cv::Mat &originalImage, joda::settings::ChannelIndex channelIndex)
      -> DetectionResponse = 0;

protected:
  /////////////////////////////////////////////////////
  auto getFilterSettings() const -> const joda::settings::ChannelSettingsFilter &
  {
    return mFilterSettings;
  }

private:
  const joda::settings::ChannelSettingsFilter &mFilterSettings;
};
}    // namespace joda::image::detect
