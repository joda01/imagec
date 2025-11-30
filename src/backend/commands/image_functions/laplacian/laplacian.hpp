///
/// \file      margin_crop.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "laplacian_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Laplacian (2D convolution)
///
class Laplacian : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  explicit Laplacian(const settings::LaplacianSettings &settings) : mSettings(settings)
  {
  }
  void execute(cv::Mat &image) override
  {
    CV_Assert(image.type() == CV_16U || image.type() == CV_32F);

    // Step 1: Convert to float if input is 16-bit, otherwise use as-is
    cv::Mat lap;
    bool inputIs16U = (image.type() == CV_16U);
    if(inputIs16U) {
      image.convertTo(lap, CV_32F);
    } else {
      lap = image;
    }

    // Step 2: Compute Laplacian in float
    for(int32_t n = 0; n < mSettings.repeat; n++) {
      cv::Laplacian(lap, lap, CV_32F, mSettings.kernelSize);
    }

    // Step 3: Rescale if input was 16-bit
    if(inputIs16U) {
      double minVal;
      double maxVal;
      cv::minMaxLoc(lap, &minVal, &maxVal);

      // Avoid division by zero for flat images
      if(minVal == maxVal) {
        lap.setTo(0);
      } else {
        // Scale lap to full 16-bit range
        lap.convertTo(lap, CV_16U, 65535.0 / (maxVal - minVal), -minVal * 65535.0 / (maxVal - minVal));
      }
    }

    image = lap;
  }

private:
  /////////////////////////////////////////////////////
  const settings::LaplacianSettings &mSettings;
};

}    // namespace joda::cmd
