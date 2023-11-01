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
/// \class      ThresholdTriangle
/// \author     Joachim Danmayr
/// \brief      Triangle thershold calculation
///
class ThresholdTriangle final : public Threshold
{
public:
  using Threshold::Threshold;

private:
  [[nodiscard]] uint16_t calcThresholdValue(const cv::Mat &srcImg) const
  {
    // Calculate the histogram of the image
    cv::Mat histogram;
    int histSize           = UINT16_MAX;         // Number of bins
    float range[]          = {0, UINT16_MAX};    // Pixel value range
    const float *histRange = {range};
    cv::calcHist(&srcImg, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);

    int threshold      = 0;
    double minVariance = std::numeric_limits<double>::max();

    for(int t = 0; t < histSize; t++) {
      double sumLower        = 0.0;
      double sumUpper        = 0.0;
      double sumLowerSquares = 0.0;
      double sumUpperSquares = 0.0;

      // Calculate sums for the lower and upper classes
      for(int i = 0; i <= t; i++) {
        sumLower += histogram.at<float>(i);
        sumLowerSquares += i * histogram.at<float>(i);
      }
      for(int i = t + 1; i < histSize; i++) {
        sumUpper += histogram.at<float>(i);
        sumUpperSquares += i * histogram.at<float>(i);
      }

      // Calculate variances for the lower and upper classes
      double varianceLower = (sumLowerSquares / sumLower) - (sumLower / sumLower);
      double varianceUpper = (sumUpperSquares / sumUpper) - (sumUpper / sumUpper);

      // Calculate the weighted sum of variances
      double weightedVariance = (sumLower * varianceLower + sumUpper * varianceUpper) / (sumLower + sumUpper);

      // Update the threshold if it minimizes the weighted variance
      if(weightedVariance < minVariance) {
        minVariance = weightedVariance;
        threshold   = t;
      }
    }

    return std::max(getMinThreshold(), static_cast<uint16_t>(threshold));
  }
};

}    // namespace joda::func::img
