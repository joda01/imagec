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

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::func::img {

///
/// \class      Threshold
/// \author     Joachim Danmayr
/// \brief      Base class for thershold calculation
///
class Threshold
{
public:
  Threshold(uint16_t minThreshold) : mMinThreshold(minThreshold)
  {
  }
  virtual ~Threshold() = default;

  virtual uint16_t execute(const cv::Mat &srcImg, cv::Mat &thresholdImg) const
  {
    auto thresholdVal = autoThreshold(srcImg);
    cv::threshold(srcImg, thresholdImg, thresholdVal, UINT16_MAX, cv::THRESH_BINARY);
    return thresholdVal;
  }

protected:
  /////////////////////////////////////////////////////
  [[nodiscard]] uint16_t getMinThreshold() const
  {
    return mMinThreshold;
  }

private:
  /////////////////////////////////////////////////////
  [[nodiscard]] virtual uint16_t calcThresholdValue(cv::Mat &histogram) const = 0;

  [[nodiscard]] virtual uint16_t autoThreshold(const cv::Mat &srcImg) const
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

    auto thresholdTemp = calcThresholdValue(histogram) + 1;

    return std::max(getMinThreshold(), static_cast<uint16_t>(scaleAndSetThreshold(0, thresholdTemp, min, max)));
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
};

}    // namespace joda::func::img