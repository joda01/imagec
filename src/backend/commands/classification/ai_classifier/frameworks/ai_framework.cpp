
///
/// \file      ai_framework.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "ai_framework.hpp"
#include <opencv2/imgproc.hpp>

namespace joda::ai {

auto AiFramework::prepareImage(const cv::Mat &inputImageOriginal, const InputParameters &settings, int colorOrder) -> cv::Mat
{
  // Normalize the pixel values to [0, 1] float for detection
  cv::Mat grayImageFloat;
  if(settings.dataType == InputParameters::NetInputType::FLOAT32) {
    inputImageOriginal.convertTo(grayImageFloat, CV_32F, 1 / 65535.0);
  } else if(settings.dataType == InputParameters::NetInputType::UINT8) {
    inputImageOriginal.convertTo(grayImageFloat, CV_32F, 255.0 / 65535.0);
  }

  if(settings.nrOfChannels == 3) {
    cv::Mat inputImage(inputImageOriginal.size(), CV_32FC3);
    cv::cvtColor(grayImageFloat, inputImage, colorOrder);
    // Resize
    cv::Mat resizedImage(cv::Size(settings.inputWidth, settings.inputHeight), CV_32FC3);
    cv::resize(inputImage, resizedImage, cv::Size(settings.inputWidth, settings.inputHeight));    // Adjust size to your model's input

    return resizedImage;
  } else {
    cv::Mat resizedImage;
    cv::resize(grayImageFloat, resizedImage, cv::Size(settings.inputWidth, settings.inputHeight));    // Adjust size to your model's input
    return resizedImage;
  }
}
}    // namespace joda::ai
