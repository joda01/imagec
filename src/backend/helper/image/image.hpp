///
/// \file      image.hpp
/// \author    Joachim Danmayr
/// \date      2024-07-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qpixmap.h>
#include <qpoint.h>
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
  ~Image()
  {
    clear();
  }
  void setImage(const cv::Mat &&imageToDisplay, int32_t rescale = 2048);
  bool empty()
  {
    if(mImageOriginal != nullptr) {
      return mImageOriginal->empty();
    }
    return true;
  }
  [[nodiscard]] const cv::Mat *getImage() const
  {
    return mImageOriginal;
  }
  struct Overlay
  {
    const Image *combineWith = nullptr;
    float opaque             = 0.3;
  };
  [[nodiscard]] QPixmap getPixmap(const Overlay &) const;

  [[nodiscard]] uint16_t getHistogramDisplayAreaLower() const
  {
    return mDisplayAreaLower;
  }
  [[nodiscard]] uint16_t getHistogramDisplayAreaUpper() const
  {
    return mDisplayAreaUpper;
  }

  [[nodiscard]] uint16_t getUpperLevelContrast() const
  {
    return mUpperValue;
  }

  [[nodiscard]] uint16_t getLowerLevelContrast() const
  {
    return mLowerValue;
  }
  void clear()
  {
    if(mImageOriginal != nullptr) {
      delete mImageOriginal;
      mImageOriginal = nullptr;
    }
  }

  auto getHistogram() const -> const cv::Mat &
  {
    return mHistogram;
  }

  void setBrightnessRange(int32_t lowerValue, int32_t upperValue, int32_t displayAreaLower, int32_t displayAreaUpper);

  struct AutoAdjustRet
  {
    uint16_t sigmaLower       = 0;    // +/- index around the maximum
    uint16_t sigmaUpper       = 0;    // +/- index around the maximum
    uint16_t histogramMaximum = 0;    // Index of the maximum
    uint16_t adjustIdx        = 0;    // Calculated "optimal" adjustment index
  };
  void autoAdjustBrightnessRange();
  auto getOriginalImageSize() const -> const QSize &
  {
    return mOriginalImageSize;
  }

  auto getPreviewImageSize() const -> QSize
  {
    if(mImageOriginal != nullptr) {
      return {mImageOriginal->cols, mImageOriginal->rows};
    } else {
      return {};
    }
  }

private:
  /////////////////////////////////////////////////////
  cv::Mat mHistogram;

  /////////////////////////////////////////////////////
  [[nodiscard]] QPixmap encode(const cv::Mat *image) const;
  std::array<int32_t, UINT16_MAX + 1> mLut = {};

  //// BRIGHTNESS /////////////////////////////////////////////////
  uint16_t mLowerValue       = 0;
  uint16_t mUpperValue       = UINT16_MAX;
  uint16_t mDisplayAreaLower = 0;
  uint16_t mDisplayAreaUpper = 0;
  QSize mOriginalImageSize;

  //// IMAGE /////////////////////////////////////////////////
  const cv::Mat *mImageOriginal = nullptr;
};

}    // namespace joda::image
