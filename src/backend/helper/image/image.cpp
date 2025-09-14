///
/// \file      image.cpp
/// \author    Joachim Danmayr
/// \date      2024-07-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "image.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <string>
#include "backend/commands/image_functions/enhance_contrast/enhance_contrast.hpp"
#include "backend/commands/image_functions/resize/resize.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::image {

///
/// \brief      Constructor
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Image::Image()
{
  for(int32_t i = 0; i < 65536; ++i) {
    mLut[static_cast<size_t>(i)] = i;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::setImage(const cv::Mat &&imageToDisplay, int32_t rescale)
{
  mOriginalImageSize = {imageToDisplay.cols, imageToDisplay.rows};
  if(rescale > 0) {
    std::lock_guard<std::mutex> lock(mLockMutex);
    delete mOriginalImage;
    mOriginalImage = new cv::Mat(imageToDisplay);
    delete mImageOriginalScaled;
    mImageOriginalScaled = new cv::Mat(joda::image::func::Resizer::resizeWithAspectRatio(imageToDisplay, std::min(imageToDisplay.cols, rescale),
                                                                                         std::min(imageToDisplay.rows, rescale)));
  } else {
    mImageOriginalScaled = &imageToDisplay;
    mOriginalImage       = &imageToDisplay;
  }
  int type  = mImageOriginalScaled->type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth == CV_16U && 1 == mImageOriginalScaled->channels()) {
    //
    // Calc histogram
    //
    int histSize           = UINT16_MAX + 1;
    float range[]          = {0, UINT16_MAX + 1};
    const float *histRange = {range};
    cv::calcHist(mImageOriginalScaled, 1, nullptr, cv::Mat(), mHistogram, 1, &histSize, &histRange);    //, uniform, accumulate);

    // Normalize the histogram to [0, histImage.height()]
    mHistogram.at<float>(0) = 0;    // We don't want to display black
    cv::normalize(mHistogram, mHistogram, 0, 1, cv::NORM_MINMAX);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QPixmap Image::getPixmap() const
{
  std::lock_guard<std::mutex> lock(mLockMutex);
  if(mImageOriginalScaled == nullptr) {
    std::cout << "Ups it is null" << std::endl;
    return {};
  }
  if(mLowerValue > mUpperValue) {
    std::cerr << "Minimum value must be less than maximum value." << std::endl;
    return {};
  }

  // 65535....65535
  // 3000 .......65535
  // PxlInImg....New
  int type  = mImageOriginalScaled->type();
  int depth = type & CV_MAT_DEPTH_MASK;
  cv::Mat imageTmp;

  // Take 2ms
  if(depth == CV_16U) {
    imageTmp = mImageOriginalScaled->clone();
    for(int y = 0; y < imageTmp.rows; ++y) {
      for(int x = 0; x < imageTmp.cols; ++x) {
        size_t pixelValue           = imageTmp.at<uint16_t>(y, x);
        imageTmp.at<uint16_t>(y, x) = static_cast<uint16_t>(mLut[pixelValue]);
      }
    }
    return encode(&imageTmp);
  } else {
    return encode(mImageOriginalScaled);
  }
  return encode(mImageOriginalScaled);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::setBrightnessRange(int32_t lowerValue, int32_t upperValue, int32_t displayAreaLower, int32_t displayAreaUpper)
{
  if(lowerValue > upperValue) {
    lowerValue = upperValue;
  }
  if(displayAreaLower < 0) {
    displayAreaLower = 0;
  }
  if(displayAreaUpper > UINT16_MAX) {
    displayAreaUpper = UINT16_MAX;
  }
  if(displayAreaLower > displayAreaUpper) {
    displayAreaLower = displayAreaUpper;
  }

  mLowerValue       = static_cast<uint16_t>(lowerValue);
  mUpperValue       = static_cast<uint16_t>(upperValue);
  mDisplayAreaLower = static_cast<uint16_t>(displayAreaLower);
  mDisplayAreaUpper = static_cast<uint16_t>(displayAreaUpper);

  // Create a lookup table for mapping pixel values
  for(size_t i = 0; i < 65536; ++i) {
    if(i < mLowerValue) {
      mLut[i] = 0;
    } else if(i > mUpperValue) {
      mLut[i] = 65535;
    } else {
      mLut[i] = static_cast<uint16_t>((static_cast<double>(i) - static_cast<double>(mLowerValue)) * 65535.0 /
                                      (static_cast<double>(mUpperValue) - static_cast<double>(mLowerValue)));
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::autoAdjustBrightnessRange()
{
  std::lock_guard<std::mutex> lock(mLockMutex);
  if(mImageOriginalScaled != nullptr) {
    if(mImageOriginalScaled->empty() || mImageOriginalScaled->channels() != 1) {
      return;
    }
    int histSize           = UINT16_MAX + 1;
    float range[]          = {0, UINT16_MAX + 1};
    const float *histRange = {range};
    cv::Mat hist;
    cv::calcHist(mImageOriginalScaled, 1, nullptr, cv::Mat(), hist, 1, &histSize, &histRange);
    auto [lowerIdx, upperIdx] = joda::cmd::EnhanceContrast::findContrastStretchBounds(hist, 0.005);
    setBrightnessRange(lowerIdx, upperIdx, lowerIdx - 256, upperIdx + 256);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QPixmap Image::encode(const cv::Mat *image) const
{
  if(mImageOriginalScaled == nullptr || image == nullptr) {
    return {};
  }

  switch(image->type()) {
    case CV_16UC1: {
      return QPixmap::fromImage(QImage(image->data, image->cols, image->rows, static_cast<uint32_t>(image->step), QImage::Format_Grayscale16));
    } break;

    case CV_8UC1: {
      return QPixmap::fromImage(QImage(image->data, image->cols, image->rows, static_cast<uint32_t>(image->step), QImage::Format_Grayscale8));
    } break;
    case CV_8UC3: {
      auto pixmap =
          QPixmap::fromImage(QImage(image->data, image->cols, image->rows, static_cast<uint32_t>(image->step), QImage::Format_RGB888).rgbSwapped());
      return pixmap;
    } break;
    case CV_8UC4: {
      return QPixmap::fromImage(QImage(image->data, image->cols, image->rows, static_cast<uint32_t>(image->step), QImage::Format_ARGB32));
    } break;
    default:
      std::cout << "Not supported" << std::endl;
      break;
  }
  return {};
}

}    // namespace joda::image
