///
/// \file      ai_segmentation.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "ai_segmentation.hpp"
#include <opencv2/imgproc.hpp>

namespace joda::ai {

auto AiSegmentation::prepareImage(const cv::Mat &inputImageOriginal, const settings::AiClassifierSettings &settings) -> cv::Mat
{
  // Normalize the pixel values to [0, 1] float for detection
  cv::Mat grayImageFloat;
  if(settings.modelInputParameters.netInputType == settings::AiClassifierSettings::NetInputType::FLOAT32) {
    inputImageOriginal.convertTo(grayImageFloat, CV_32F, 1.0 / 65535.0);
  } else if(settings.modelInputParameters.netInputType == settings::AiClassifierSettings::NetInputType::UINT8) {
    inputImageOriginal.convertTo(grayImageFloat, CV_32F, 255.0 / 65535.0);
  }

  cv::Mat inputImage;
  if(settings.modelInputParameters.netNrOfChannels == settings::AiClassifierSettings::NetChannels::RGB) {
    cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);
  } else {
    inputImage = grayImageFloat;
  }

  // Resize
  cv::Mat resizedImage;
  cv::resize(
      inputImage, resizedImage,
      cv::Size(settings.modelInputParameters.netInputWidth, settings.modelInputParameters.netInputHeight));    // Adjust size to your model's input

  return resizedImage;
}

}    // namespace joda::ai
