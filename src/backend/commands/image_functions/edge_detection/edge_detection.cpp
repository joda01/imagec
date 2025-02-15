///
/// \file      edge_detecion.cpp
/// \author
/// \date      2023-10-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     C++ implementation of Blur algorithm based on
///            ImageJ short processor implementation
///
/// \ref       https://github.com/imagej/ImageJ/blob/master/ij/process/ShortProcessor.java
///

#include "edge_detection.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::cmd {

void EdgeDetection::canny(cv::Mat &image) const
{
  cv::Mat binaryImage(image.size(), CV_8UC1);
  image.convertTo(binaryImage, CV_8UC1, 255.0 / 65535.0);
  cv::Canny(binaryImage, binaryImage, mSetting.thresholdMin, mSetting.thresholdMax, mSetting.kernelSize);
  binaryImage.convertTo(image, CV_16UC1, 257.0);
}

void EdgeDetection::filter3x3(cv::Mat &image) const
{
  int v1    = 0;
  int v2    = 0;
  int v3    = 0;    // input pixel values around the current pixel
  int v4    = 0;
  int v5    = 0;
  int v6    = 0;
  int v7    = 0;
  int v8    = 0;
  int v9    = 0;
  int k1    = 0;
  int k2    = 0;
  int k3    = 0;    // kernel values (used for CONVOLVE only)
  int k4    = 0;
  int k5    = 0;
  int k6    = 0;
  int k7    = 0;
  int k8    = 0;
  int k9    = 0;
  int scale = 0;

  int roiX      = 0;
  int roiY      = 0;
  int roiWidth  = image.cols;
  int roiHeight = image.rows;
  int width     = image.cols;
  int height    = image.rows;

  cv::Mat imageCopy = image.clone();
  int xEnd          = roiX + roiWidth;
  int yEnd          = roiY + roiHeight;
  for(int y = roiY; y < yEnd; y++) {
    int p  = roiX + y * width;                     // points to current pixel
    int p6 = p - (roiX > 0 ? 1 : 0);               // will point to v6, currently lower
    int p3 = p6 - (y > 0 ? width : 0);             // will point to v3, currently lower
    int p9 = p6 + (y < height - 1 ? width : 0);    // ...  to v9, currently lower
    v2     = imageCopy.at<unsigned short>(p3) & 0xffff;
    v5     = imageCopy.at<unsigned short>(p6) & 0xffff;
    v8     = imageCopy.at<unsigned short>(p9) & 0xffff;
    if(roiX > 0) {
      p3++;
      p6++;
      p9++;
    }
    v3 = imageCopy.at<unsigned short>(p3) & 0xffff;
    v6 = imageCopy.at<unsigned short>(p6) & 0xffff;
    v9 = imageCopy.at<unsigned short>(p9) & 0xffff;

    for(int x = roiX; x < xEnd; x++, p++) {
      if(x < width - 1) {
        p3++;
        p6++;
        p9++;
      }
      v1            = v2;
      v2            = v3;
      v3            = imageCopy.at<unsigned short>(p3) & 0xffff;
      v4            = v5;
      v5            = v6;
      v6            = imageCopy.at<unsigned short>(p6) & 0xffff;
      v7            = v8;
      v8            = v9;
      v9            = imageCopy.at<unsigned short>(p9) & 0xffff;
      double sum1   = v1 + 2 * v2 + v3 - v7 - 2 * v8 - v9;
      double sum2   = v1 + 2 * v4 + v7 - v3 - 2 * v6 - v9;
      double result = std::sqrt(sum1 * sum1 + sum2 * sum2);
      if(result > 65535.0)
        result = 65535.0;
      image.at<unsigned short>(p) = static_cast<unsigned short>(result);
    }
  }
}

}    // namespace joda::cmd
