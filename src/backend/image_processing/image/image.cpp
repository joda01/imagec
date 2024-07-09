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
    toPixmap(image);
  } else {
    toPixmap(mImageOriginal);
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
std::vector<uchar> Image::encodeToPNG(const cv::Mat &image)
{
  cv::Mat originalImageFloat;
  int type  = image.type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth != CV_32F) {
    cv::Mat grayImageFloat;
    image.convertTo(grayImageFloat, CV_32FC3, (float) UINT8_MAX / (float) UINT16_MAX);
    cv::cvtColor(grayImageFloat, originalImageFloat, cv::COLOR_GRAY2BGR);
  } else {
    originalImageFloat = image;
  }

  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(0);
  std::vector<uchar> buffer;
  cv::imencode(".png", originalImageFloat, buffer, compression_params);
  return buffer;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::toPixmap(const cv::Mat &image)
{
  auto data = encodeToPNG(image);
  mPixmap   = {};
  mPixmap.loadFromData(data.data(), data.size(), "PNG");
}

}    // namespace joda::image
