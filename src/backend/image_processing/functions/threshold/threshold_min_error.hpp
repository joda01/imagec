///
/// \file      threshold.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <algorithm>
#include <cstdint>
#include "threshold.hpp"

namespace joda::func::img {

///
/// \class      ThresholdMin
/// \author     Joachim Danmayr
/// \brief      Minimum error thershold calculation
///
class ThresholdMin final : public Threshold
{
public:
  using Threshold::Threshold;

private:
  [[nodiscard]] uint16_t calcThresholdValue(const cv::Mat &srcImg) const
  {
    int histSize           = UINT16_MAX;         // Number of bins
    float range[]          = {0, UINT16_MAX};    // Pixel value range
    const float *histRange = {range};

    // Calculate the histogram of the image
    cv::Mat histogram;
    cv::calcHist(&srcImg, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);

    // Calculate the total sum of pixel values
    double totalSum = cv::sum(srcImg)[0];

    // Initialize variables for threshold calculation
    int threshold   = 0;
    double minError = std::numeric_limits<double>::max();

    double sumLower = 0.0;
    double sumUpper = totalSum;

    for(int t = 0; t < histSize; t++) {
      double pixelCount = histogram.at<float>(t);
      sumLower += t * pixelCount;
      sumUpper -= t * pixelCount;

      double meanLower = sumLower / cv::sum(pixelCount)[0];
      double meanUpper = sumUpper / cv::sum(pixelCount)[0];

      double error = meanLower - meanUpper;
      error *= error;
      error *= sumLower * sumUpper;

      if(error < minError) {
        minError  = error;
        threshold = t;
      }
    }

    return std::max(getMinThreshold(), static_cast<uint16_t>(threshold));
  }
};

}    // namespace joda::func::img
