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
  mEditedImage   = imageToDisplay;
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
  if(depth != CV_32F) {
    std::cout << "Upper " << std::to_string(mUpperValue) << std::endl;
    mEditedImage = mImageOriginal * (float) UINT16_MAX / (float) mUpperValue;
  }

  toPixmap();
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
std::vector<uchar> Image::encodeToPNG()
{
  cv::Mat originalImageFloat;
  int type  = mEditedImage.type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth != CV_32F) {
    cv::Mat grayImageFloat;
    mEditedImage.convertTo(grayImageFloat, CV_32FC3, (float) UINT8_MAX / (float) UINT16_MAX);
    cv::cvtColor(grayImageFloat, originalImageFloat, cv::COLOR_GRAY2BGR);
  } else {
    originalImageFloat = mEditedImage;
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
void Image::toPixmap()
{
  auto data = encodeToPNG();
  mPixmap.loadFromData(data.data(), data.size(), "PNG");
}

}    // namespace joda::image
