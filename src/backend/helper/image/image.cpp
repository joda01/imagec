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
  for(int i = 0; i < 65536; ++i) {
    mLut[i] = i;
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
    delete mImageOriginal;
    mImageOriginal = new cv::Mat(joda::image::func::Resizer::resizeWithAspectRatio(imageToDisplay, std::min(imageToDisplay.cols, rescale),
                                                                                   std::min(imageToDisplay.rows, rescale)));
  } else {
    mImageOriginal = &imageToDisplay;
  }
  int type  = mImageOriginal->type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth == CV_16U && 1 == mImageOriginal->channels()) {
    //
    // Calc histogram
    //
    int histSize           = UINT16_MAX + 1;
    float range[]          = {0, UINT16_MAX + 1};
    const float *histRange = {range};
    bool uniform           = true;
    bool accumulate        = false;
    cv::calcHist(mImageOriginal, 1, 0, cv::Mat(), mHistogram, 1, &histSize, &histRange);    //, uniform, accumulate);

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
QPixmap Image::getPixmap(const Overlay &overlay) const
{
  std::lock_guard<std::mutex> lock(mLockMutex);
  if(mImageOriginal == nullptr) {
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
  int type  = mImageOriginal->type();
  int depth = type & CV_MAT_DEPTH_MASK;
  cv::Mat image;

  // Take 2ms
  if(depth == CV_16U) {
    image = mImageOriginal->clone();
    for(int y = 0; y < image.rows; ++y) {
      for(int x = 0; x < image.cols; ++x) {
        uint16_t pixelValue      = image.at<uint16_t>(y, x);
        image.at<uint16_t>(y, x) = mLut[pixelValue];
      }
    }
    // Takes 20ms
    if(overlay.combineWith != nullptr) {
      // Convert 16-bit grayscale to 8-bit grayscale
      image.convertTo(image, CV_8UC3, 255.0 / 65535.0);    // Normalize to 8-bit
      cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

      // Add transparent effect
      cv::Mat coloredImage = overlay.combineWith->mImageOriginal->clone();
      int numPixels        = image.rows * image.cols;
      for(int64_t n = 0; n < numPixels; n++) {
        auto &original = image.at<cv::Vec3b>(n);
        auto &mask     = coloredImage.at<cv::Vec3b>(n);
        if(mask == cv::Vec3b{0, 0, 0}) {
          mask = original;
        } else {
          for(int c = 0; c < 3; ++c) {
            mask[c] = static_cast<uchar>(static_cast<float>(mask[c]) * overlay.opaque + static_cast<float>(original[c]) * (1.0f - overlay.opaque));
          }
        }
      }

      return encode(&coloredImage);
    } else {
      return encode(&image);
    }
  } else {
    if(overlay.combineWith != nullptr) {
      cv::Mat image = mImageOriginal->clone();
      int numPixels = image.rows * image.cols;
      for(int64_t n = 0; n < numPixels; n++) {
        auto &original = image.at<cv::Vec3b>(n);
        auto &mask     = overlay.combineWith->mImageOriginal->at<cv::Vec3b>(n);
        if(mask == cv::Vec3b{0, 0, 0}) {
          original = original;
        } else {
          for(int c = 0; c < 3; ++c) {
            original[c] =
                static_cast<uchar>(static_cast<float>(mask[c]) * overlay.opaque + static_cast<float>(original[c]) * (1.0f - overlay.opaque));
          }
        }
      }
      return encode(&image);
    } else {
      return encode(mImageOriginal);
    }
  }
  return encode(mImageOriginal);
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

  mLowerValue       = lowerValue;
  mUpperValue       = upperValue;
  mDisplayAreaLower = displayAreaLower;
  mDisplayAreaUpper = displayAreaUpper;

  // Create a lookup table for mapping pixel values
  for(int i = 0; i < 65536; ++i) {
    if(i < mLowerValue) {
      mLut[i] = 0;
    } else if(i > mUpperValue) {
      mLut[i] = 65535;
    } else {
      mLut[i] = static_cast<uint16_t>((i - static_cast<float>(mLowerValue)) * 65535.0 /
                                      (static_cast<float>(mUpperValue) - static_cast<float>(mLowerValue)));
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
  if(mImageOriginal != nullptr) {
    if(mImageOriginal->empty() || mImageOriginal->channels() != 1) {
      return;
    }
    int histSize           = UINT16_MAX + 1;
    float range[]          = {0, UINT16_MAX + 1};
    const float *histRange = {range};
    bool uniform           = true;
    bool accumulate        = false;
    cv::Mat hist;
    cv::calcHist(mImageOriginal, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);    //, uniform, accumulate);
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
  if(mImageOriginal == nullptr || image == nullptr) {
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
