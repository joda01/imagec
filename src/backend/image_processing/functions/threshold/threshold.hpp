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

  uint16_t execute(const cv::Mat &srcImg, const cv::Mat &thresholdImg) const
  {
    auto thresholdVal = calcThresholdValue(srcImg);
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
  [[nodiscard]] virtual uint16_t calcThresholdValue(const cv::Mat &srcImg) const = 0;

  /////////////////////////////////////////////////////
  uint16_t mMinThreshold = 0;
};

}    // namespace joda::func::img
