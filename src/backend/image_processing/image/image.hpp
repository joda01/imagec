///
/// \file      image.hpp
/// \author    Joachim Danmayr
/// \date      2024-07-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qpixmap.h>
#include <cstdint>
#include <vector>
#include <opencv2/core/mat.hpp>

namespace joda::image {

///
/// \class      Image
/// \author     Joachim Danmayr
/// \brief      Class representing an image instance
///
class Image
{
public:
  /////////////////////////////////////////////////////
  Image();
  void setImage(const cv::Mat &imageToDisplay);
  bool empty()
  {
    return mImageOriginal.empty();
  }
  [[nodiscard]] const cv::Mat &getImage() const
  {
    return mImageOriginal;
  }
  [[nodiscard]] const QPixmap &getPixmap() const
  {
    return mPixmap;
  }
  [[nodiscard]] float getHitogramZoomFactor() const
  {
    return mHistogramZoomFactor;
  }
  [[nodiscard]] float getHistogramOffset() const
  {
    return mHistogramOffset;
  }

  [[nodiscard]] uint16_t getUpperLevelContrast() const
  {
    return mUpperValue;
  }

  void setBrightnessRange(uint16_t lowerValue, uint16_t upperValue, float histogramZoomFactor,
                          uint16_t histogramOffset);

private:
  /////////////////////////////////////////////////////
  void update();
  void encode(const cv::Mat &image);

  //// BRIGHTNESS /////////////////////////////////////////////////
  uint16_t mLowerValue       = 0;
  uint16_t mUpperValue       = UINT16_MAX;
  float mHistogramZoomFactor = 1;
  float mHistogramOffset     = 0;

  //// IMAGE /////////////////////////////////////////////////
  cv::Mat mImageOriginal;
  QPixmap mPixmap;
};

}    // namespace joda::image
