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
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::setImage(const cv::Mat &&imageToDisplay)
{
  delete mImageOriginal;
  mImageOriginal = new cv::Mat(
      joda::image::func::Resizer::resizeWithAspectRatio(imageToDisplay, std::min(imageToDisplay.cols, WIDTH), std::min(imageToDisplay.rows, WIDTH)));
  int type  = mImageOriginal->type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth == CV_16U) {
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
QPixmap Image::getPixmap(const Image *combineWith) const
{
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
    if(combineWith != nullptr) {
      // Convert 16-bit grayscale to 8-bit grayscale
      image.convertTo(image, CV_8U, 255.0 / 65535.0);    // Normalize to 8-bit
      cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

      cv::Mat mask;
      cv::Mat coloredImage = combineWith->mImageOriginal->clone();
      cv::inRange(coloredImage, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
      image.copyTo(coloredImage, mask);
      return encode(&coloredImage);

    } else {
      return encode(&image);
    }
  } else {
    if(combineWith != nullptr) {
      cv::Mat image = mImageOriginal->clone();
      cv::bitwise_xor(image, *combineWith->mImageOriginal, image);
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
void Image::setBrightnessRange(uint16_t lowerValue, uint16_t upperValue, float histogramZoomFactor, uint16_t histogramOffset)
{
  mLowerValue          = lowerValue;
  mUpperValue          = upperValue;
  mHistogramZoomFactor = histogramZoomFactor;
  mHistogramOffset     = histogramOffset;

  // Create a lookup table for mapping pixel values
  for(int i = 0; i < 65535; ++i) {
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
    mLut = joda::cmd::EnhanceContrast::equalize(hist);
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
