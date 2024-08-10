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
#include "backend/image_processing/functions/watershed/watershed.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::image::func {

///
/// \class      Threshold
/// \author     Joachim Danmayr
/// \brief      Base class for thershold calculation
///
class Threshold
{
public:
  Threshold(uint16_t minThreshold, uint16_t maxThreshold) : mMinThreshold(minThreshold), mMaxThreshold(maxThreshold)
  {
  }
  virtual ~Threshold() = default;

  virtual std::tuple<uint16_t, uint16_t> execute(const cv::Mat &srcImg, cv::Mat &thresholdImg) const
  {
    auto [thresholdValMin, thresholdValMax] = autoThreshold(srcImg);
    cv::threshold(srcImg, thresholdImg, thresholdValMin, UINT16_MAX, cv::THRESH_BINARY);
    cv::Mat thresholdTmp;
    cv::threshold(srcImg, thresholdTmp, thresholdValMax, UINT16_MAX, cv::THRESH_BINARY_INV);
    cv::bitwise_and(thresholdImg, thresholdTmp, thresholdImg);

    imwrite("output.png", thresholdImg);

    // Watershed watershed;
    // watershed.execute(thresholdImg);
    return {thresholdValMin, thresholdValMax};
  }

protected:
  /////////////////////////////////////////////////////
  [[nodiscard]] uint16_t getMinThreshold() const
  {
    return mMinThreshold;
  }

  [[nodiscard]] uint16_t getMaxThreshold() const
  {
    return mMaxThreshold;
  }

private:
  /////////////////////////////////////////////////////
  [[nodiscard]] virtual uint16_t calcThresholdValue(cv::Mat &histogram) const = 0;

  [[nodiscard]] virtual std::tuple<uint16_t, uint16_t> autoThreshold(const cv::Mat &srcImg) const
  {
    //
    // Scale image
    //
    double min;
    double max;
    cv::Point min_loc;
    cv::Point max_loc;
    // Find the minimum and maximum values and their locations
    cv::minMaxLoc(srcImg, &min, &max, &min_loc, &max_loc);
    double scale    = 256.0 / (max - min + 1);
    cv::Mat charImg = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC1);
    // srcImg.convertTo(charImg, CV_8UC1, scale);
    for(int i = 0; i < srcImg.total(); i++) {
      auto value = (srcImg.at<uint16_t>(i) & 0xffff) - min;
      if(value < 0) {
        value = 0;
      }
      value = (int) (value * scale + 0.5);
      if(value > 255) {
        value = 255;
      }
      charImg.at<uint8_t>(i) = (uint8_t) value;
    }

    // Calculate the histogram of the image
    int histSize           = UINT8_MAX + 1;    // Number of bins
    float range[]          = {0, 256};         // Pixel value range
    const float *histRange = {range};
    cv::Mat histogram;
    cv::calcHist(&charImg, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);
    // histogram.at<float>(0) = 0;

    auto thresholdTempMin = scaleAndSetThreshold(0, calcThresholdValue(histogram) + 1, min, max);

    return {std::min(std::max(getMinThreshold(), thresholdTempMin), getMaxThreshold()), getMaxThreshold()};
  }

  /////////////////////////////////////////////////////

  ///
  /// \ref https://imagej.net/ij/developer/source/ij/process/ImageProcessor.java.html
  ///
  [[nodiscard]] uint16_t scaleAndSetThreshold(double lower, double upper, double min, double max) const
  {
    if(max > min) {
      if(lower == 0.0) {
      } else {
        lower = min + (lower / 255.0) * (max - min);
      }
      if(upper == 255.0) {
        upper = 65535;
      } else {
        upper = min + (upper / 255.0) * (max - min);
      }
    } else {
      lower = upper = min;
    }
    return upper;
  }

  /////////////////////////////////////////////////////
  uint16_t mMinThreshold = 0;
  uint16_t mMaxThreshold = 0;
};

}    // namespace joda::image::func
