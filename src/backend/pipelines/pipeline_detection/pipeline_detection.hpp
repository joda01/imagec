///
/// \file      pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2023-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Abstract base class for a pipeline run
///

#pragma once

#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/settings/channel_settings.hpp"
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

namespace joda::pipeline::detection {

///
/// \class      Pipeline
/// \author     Joachim Danmayr
/// \brief      Pipeline base class
///
class Detection
{
public:
  /////////////////////////////////////////////////////
  Detection(const std::map<std::string, joda::onnx::OnnxParser::Data> &models) : mAvailableModels(models)
  {
  }

  virtual auto execute(const cv::Mat &img, const cv::Mat &imgOriginal,
                       const joda::settings::json::ChannelSettings &channelSetting) -> func::DetectionResponse = 0;

  [[nodiscard]] auto getAvailableModels() const -> const std::map<std::string, joda::onnx::OnnxParser::Data> &
  {
    return mAvailableModels;
  }

private:
  /////////////////////////////////////////////////////
  std::map<std::string, joda::onnx::OnnxParser::Data> mAvailableModels;
};
}    // namespace joda::pipeline::detection
