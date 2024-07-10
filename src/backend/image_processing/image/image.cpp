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
#include <string>
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
void Image::setImage(const cv::Mat &imageToDisplay)
{
  mImageOriginal = imageToDisplay;
  update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::update()
{
  // 65535....65535
  // 3000 .......65535
  // PxlInImg....New
  int type  = mImageOriginal.type();
  int depth = type & CV_MAT_DEPTH_MASK;
  cv::Mat image;
  if(depth != CV_32F) {
    image = mImageOriginal.clone();
    // Ensure minVal is less than maxVal
    if(mLowerValue >= mUpperValue) {
      std::cerr << "Minimum value must be less than maximum value." << std::endl;
      return;
    }

    // Create a lookup table for mapping pixel values
    cv::Mat lookupTable(1, 65535, CV_16U);

    for(int i = 0; i < 65535; ++i) {
      if(i < mLowerValue) {
        lookupTable.at<uint16_t>(i) = 0;
      } else if(i > mUpperValue) {
        lookupTable.at<uint16_t>(i) = 65535;
      } else {
        lookupTable.at<uint16_t>(i) = static_cast<uint16_t>((i - mLowerValue) * 65535.0 / (mUpperValue - mLowerValue));
      }
    }

    // Apply the lookup table to the source image to get the destination image
    for(int y = 0; y < image.rows; ++y) {
      for(int x = 0; x < image.cols; ++x) {
        uint16_t pixelValue      = image.at<uint16_t>(y, x);
        image.at<uint16_t>(y, x) = lookupTable.at<uint16_t>(pixelValue);
      }
    }
    encode(image);
  } else {
    encode(mImageOriginal);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::setBrightnessRange(int32_t lowerValue, int32_t upperValue)
{
  mLowerValue = lowerValue;
  mUpperValue = upperValue;
  update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::encode(const cv::Mat &image)
{
  cv::Mat originalImageFloat;
  int type  = image.type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(image.type() == CV_16UC1) {
    cv::Mat img8;
    image.convertTo(img8, CV_8UC1, 1.0 / 256.0);    // scaling factor to convert 16-bit to 8-bit
    cv::cvtColor(img8, originalImageFloat, cv::COLOR_GRAY2BGR);
  } else if(image.type() == CV_32FC3) {
    image.convertTo(originalImageFloat, CV_8UC3, 1);    // scaling factor to convert 16-bit to 8-bit
  } else if(image.type() == CV_8UC3) {
    originalImageFloat = image;
  } else {
    std::cout << "Not supported 1" << std::endl;
  }

  switch(originalImageFloat.type()) {
    case CV_8UC1: {
      mPixmap = QPixmap::fromImage(QImage(originalImageFloat.data, originalImageFloat.cols, originalImageFloat.rows,
                                          static_cast<int>(originalImageFloat.step), QImage::Format_Grayscale8));
    } break;
    case CV_32FC3:
    case CV_8UC3: {
      mPixmap = QPixmap::fromImage(QImage(originalImageFloat.data, originalImageFloat.cols, originalImageFloat.rows,
                                          static_cast<int>(originalImageFloat.step), QImage::Format_RGB888)
                                       .rgbSwapped());
    } break;
    case CV_8UC4: {
      mPixmap = QPixmap::fromImage(QImage(originalImageFloat.data, originalImageFloat.cols, originalImageFloat.rows,
                                          static_cast<int>(originalImageFloat.step), QImage::Format_ARGB32));
    } break;
    default:
      std::cout << "Not supported" << std::endl;
      break;
  }
}

}    // namespace joda::image
