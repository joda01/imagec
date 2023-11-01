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
/// \brief      LI thershold calculation
///
class ThresholdLi final : public Threshold
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
    int threshold          = 0;
    double minCrossEntropy = std::numeric_limits<double>::max();

    for(int t = 0; t < histSize; t++) {
      double pixelCount = histogram.at<float>(t);

      double sumLower  = 0.0;
      double sumUpper  = 0.0;
      double meanLower = 0.0;
      double meanUpper = 0.0;

      for(int i = 0; i <= t; i++) {
        sumLower += histogram.at<float>(i);
        meanLower += i * histogram.at<float>(i);
      }

      if(sumLower > 0) {
        meanLower /= sumLower;
      }

      for(int i = t + 1; i < histSize; i++) {
        sumUpper += histogram.at<float>(i);
        meanUpper += i * histogram.at<float>(i);
      }

      if(sumUpper > 0) {
        meanUpper /= sumUpper;
      }

      if(sumLower > 0 && sumUpper > 0) {
        double crossEntropy = -sumLower * log(meanLower) - sumUpper * log(meanUpper);
        if(crossEntropy < minCrossEntropy) {
          minCrossEntropy = crossEntropy;
          threshold       = t;
        }
      }
    }

    return std::max(getMinThreshold(), static_cast<uint16_t>(threshold));
  }
};

}    // namespace joda::func::img
