///
/// \file      blur.cpp
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

#include "blur.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::cmd::functions {

void Blur::filter3x3(cv::Mat &image, int type, int *kernel, int kernelArraySize) const
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
  if(type == CONVOLVE) {
    k1 = kernel[0];
    k2 = kernel[1];
    k3 = kernel[2];
    k4 = kernel[3];
    k5 = kernel[4];
    k6 = kernel[5];
    k7 = kernel[6];
    k8 = kernel[7];
    k9 = kernel[8];
    for(int i = 0; i < kernelArraySize; i++)
      scale += kernel[i];
    if(scale == 0)
      scale = 1;
  }
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

    switch(type) {
      case BLUR_MORE:
        for(int x = roiX; x < xEnd; x++, p++) {
          if(x < width - 1) {
            p3++;
            p6++;
            p9++;
          }
          v1                          = v2;
          v2                          = v3;
          v3                          = imageCopy.at<unsigned short>(p3) & 0xffff;
          v4                          = v5;
          v5                          = v6;
          v6                          = imageCopy.at<unsigned short>(p6) & 0xffff;
          v7                          = v8;
          v8                          = v9;
          v9                          = imageCopy.at<unsigned short>(p9) & 0xffff;
          image.at<unsigned short>(p) = (unsigned short) ((v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9 + 4) / 9);
        }
        break;
      case FIND_EDGES:
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
          image.at<unsigned short>(p) = (short) result;
        }
        break;
      case CONVOLVE:
        for(int x = roiX; x < xEnd; x++, p++) {
          if(x < width - 1) {
            p3++;
            p6++;
            p9++;
          }
          v1      = v2;
          v2      = v3;
          v3      = imageCopy.at<unsigned short>(p3) & 0xffff;
          v4      = v5;
          v5      = v6;
          v6      = imageCopy.at<unsigned short>(p6) & 0xffff;
          v7      = v8;
          v8      = v9;
          v9      = imageCopy.at<unsigned short>(p9) & 0xffff;
          int sum = k1 * v1 + k2 * v2 + k3 * v3 + k4 * v4 + k5 * v5 + k6 * v6 + k7 * v7 + k8 * v8 + k9 * v9;
          sum     = (sum + scale / 2) / scale;    // scale/2 for rounding
          if(sum > 65535)
            sum = 65535;
          if(sum < 0)
            sum = 0;
          image.at<unsigned short>(p) = (short) sum;
        }
        break;
    }
  }
}

}    // namespace joda::cmd::functions
