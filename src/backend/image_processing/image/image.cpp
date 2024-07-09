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

void Image::setImage(const cv::Mat &imageToDisplay)
{
  mImageOriginal = imageToDisplay;
  mEditedImage   = imageToDisplay;
  toPixmap();
}

std::vector<uchar> Image::encodeToPNG()
{
  cv::Mat originalImageFloat;
  int type  = mEditedImage.type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth != CV_32F) {
    cv::Mat grayImageFloat;
    mEditedImage.convertTo(grayImageFloat, CV_32F, (float) UCHAR_MAX / (float) UINT16_MAX);
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

void Image::toPixmap()
{
  auto data = encodeToPNG();
  QByteArray byteArray(reinterpret_cast<const char *>(data.data()), data.size());
  QImage originalImage;
  originalImage.loadFromData(byteArray, "PNG");
  mPixmap = QPixmap::fromImage(originalImage);
}

}    // namespace joda::image
