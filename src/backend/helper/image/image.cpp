///
/// \file      image.cpp
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

#include "image.hpp"
#include <cstdint>
#include <iostream>
#include <mutex>
#include <string>
#include "backend/commands/functions/resize/resize.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
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
  mImageOriginal = new cv::Mat(joda::image::func::Resizer::resizeWithAspectRatio(imageToDisplay, WIDTH, WIDTH));
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
  if(mImageOriginal == nullptr) {
    std::cout << "Ups it is null" << std::endl;
    return {};
  }
  // 65535....65535
  // 3000 .......65535
  // PxlInImg....New
  int type  = mImageOriginal->type();
  int depth = type & CV_MAT_DEPTH_MASK;
  cv::Mat image;
  if(depth == CV_16U) {
    image = mImageOriginal->clone();
    // Ensure minVal is less than maxVal
    if(mLowerValue >= mUpperValue) {
      std::cerr << "Minimum value must be less than maximum value." << std::endl;
      return {};
    }

    // Create a lookup table for mapping pixel values
    cv::Mat lookupTable(1, 65535, CV_16U);

    for(int i = 0; i < 65535; ++i) {
      if(i < mLowerValue) {
        lookupTable.at<uint16_t>(i) = 0;
      } else if(i > mUpperValue) {
        lookupTable.at<uint16_t>(i) = 65535;
      } else {
        lookupTable.at<uint16_t>(i) =
            static_cast<uint16_t>((i - static_cast<float>(mLowerValue)) * 65535.0 /
                                  (static_cast<float>(mUpperValue) - static_cast<float>(mLowerValue)));
      }
    }

    // Apply the lookup table to the source image to get the destination image
    for(int y = 0; y < image.rows; ++y) {
      for(int x = 0; x < image.cols; ++x) {
        uint16_t pixelValue      = image.at<uint16_t>(y, x);
        image.at<uint16_t>(y, x) = lookupTable.at<uint16_t>(pixelValue);
      }
    }
    return encode(&image);
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
void Image::setBrightnessRange(uint16_t lowerValue, uint16_t upperValue, float histogramZoomFactor,
                               uint16_t histogramOffset)
{
  mLowerValue          = lowerValue;
  mUpperValue          = upperValue;
  mHistogramZoomFactor = histogramZoomFactor;
  mHistogramOffset     = histogramOffset;
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
      return QPixmap::fromImage(QImage(image->data, image->cols, image->rows, static_cast<uint32_t>(image->step),
                                       QImage::Format_Grayscale16));
    } break;

    case CV_8UC1: {
      return QPixmap::fromImage(
          QImage(image->data, image->cols, image->rows, static_cast<uint32_t>(image->step), QImage::Format_Grayscale8));
    } break;
    case CV_8UC3: {
      auto pixmap = QPixmap::fromImage(
          QImage(image->data, image->cols, image->rows, static_cast<uint32_t>(image->step), QImage::Format_RGB888)
              .rgbSwapped());
      return pixmap;
    } break;
    case CV_8UC4: {
      return QPixmap::fromImage(
          QImage(image->data, image->cols, image->rows, static_cast<uint32_t>(image->step), QImage::Format_ARGB32));
    } break;
    default:
      std::cout << "Not supported" << std::endl;
      break;
  }
  return {};
}

}    // namespace joda::image
