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
#include "weighted_deviation_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian WeightedDeviation (2D convolution)
///
class WeightedDeviation : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  explicit WeightedDeviation(const settings::WeightedDeviationSettings &settings) : mSettings(settings)
  {
  }
  void execute(cv::Mat &image) override
  {
    CV_Assert(image.type() == CV_16U || image.type() == CV_32F);

    const int32_t ksize = mSettings.kernelSize;
    const double sigma  = mSettings.sigma;

    // Step 1: Convert 16-bit to float internally
    cv::Mat grayF;
    bool inputIs16U = (image.type() == CV_16U);
    if(inputIs16U) {
      image.convertTo(grayF, CV_32F);
    } else {
      grayF = image;
    }

    // Step 2: Gaussian smoothing
    cv::Mat mean;
    cv::Mat meanSq;
    cv::GaussianBlur(grayF, mean, cv::Size(ksize, ksize), sigma);
    cv::GaussianBlur(grayF.mul(grayF), meanSq, cv::Size(ksize, ksize), sigma);

    // Step 3: Compute standard deviation
    cv::Mat stdWeightedF;
    cv::sqrt(meanSq - mean.mul(mean), stdWeightedF);

    // Step 4: Convert back to input type if needed
    if(inputIs16U) {
      double minVal;
      double maxVal;
      cv::minMaxLoc(stdWeightedF, &minVal, &maxVal);

      // Map range [minVal, maxVal] to [0, 65535]
      if(minVal == maxVal) {
        stdWeightedF.setTo(0);
      } else {
        stdWeightedF.convertTo(stdWeightedF, CV_16U, 65535.0 / (maxVal - minVal), -minVal * 65535.0 / (maxVal - minVal));
      }
    }

    image = stdWeightedF;    // CV_32F for float input, CV_16U for 16-bit input
  }

private:
  /////////////////////////////////////////////////////
  const settings::WeightedDeviationSettings &mSettings;
};

}    // namespace joda::cmd
