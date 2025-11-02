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

#include "edge_detection_sobel.hpp"
#include "backend/commands/image_functions/edge_detection_sobel/edge_detection_sobel_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

void EdgeDetectionSobel::sobel(cv::Mat &image) const
{
  CV_Assert(image.type() == CV_16U || image.type() == CV_32F);

  // Detect input type
  bool inputIs16U = (image.type() == CV_16U);

  // Convert to float if input is 16-bit
  cv::Mat imageF;
  if(inputIs16U) {
    image.convertTo(imageF, CV_32F);
  } else {
    imageF = image;
  }

  // Compute Sobel derivatives
  cv::Mat gradX;
  cv::Mat gradY;
  int ddepth = CV_32F;    // Use float to handle negative gradients safely
  int scale  = 1;
  int delta  = 0;

  if(mSetting.derivativeOrderX > 0) {
    cv::Sobel(imageF, gradX, ddepth, mSetting.derivativeOrderX, 0, mSetting.kernelSize, scale, delta, cv::BORDER_DEFAULT);
  }
  if(mSetting.derivativeOrderY > 0) {
    cv::Sobel(imageF, gradY, ddepth, 0, mSetting.derivativeOrderY, mSetting.kernelSize, scale, delta, cv::BORDER_DEFAULT);
  }

  cv::Mat edgeImage;

  if(mSetting.weighFunction == settings::EdgeDetectionSobelSettings::WeightFunction::ABS) {
    // ABS weighting: average of absolute gradients
    cv::Mat absGradX;
    cv::Mat absGradY;
    cv::absdiff(gradX, cv::Scalar::all(0), absGradX);
    cv::absdiff(gradY, cv::Scalar::all(0), absGradY);
    cv::addWeighted(absGradX, 0.5, absGradY, 0.5, 0, edgeImage);
  } else if(mSetting.weighFunction == settings::EdgeDetectionSobelSettings::WeightFunction::MAGNITUDE) {
    // Magnitude weighting
    cv::magnitude(gradX, gradY, edgeImage);
  }

  // Convert back to original type
  if(inputIs16U) {
    // For 16-bit, scale and clip to [0,65535]
    double minVal;
    double maxVal;
    cv::minMaxLoc(edgeImage, &minVal, &maxVal);

    if(minVal == maxVal) {
      edgeImage.setTo(0);
    } else {
      edgeImage.convertTo(image, CV_16U, 65535.0 / (maxVal - minVal), -minVal * 65535.0 / (maxVal - minVal));
    }
  } else {
    image = edgeImage;    // CV_32F: keep float output
  }
}

void EdgeDetectionSobel::filter3x3(cv::Mat &image) const
{
  int v1 = 0;
  int v2 = 0;
  int v3 = 0;    // input pixel values around the current pixel
  int v4 = 0;
  int v5 = 0;
  int v6 = 0;
  int v7 = 0;
  int v8 = 0;
  int v9 = 0;
  // int k1    = 0;
  // int k2    = 0;
  // int k3    = 0;    // kernel values (used for CONVOLVE only)
  // int k4    = 0;
  // int k5    = 0;
  // int k6    = 0;
  // int k7    = 0;
  // int k8    = 0;
  // int k9    = 0;
  // int scale = 0;

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
    v2     = imageCopy.at<uint16_t>(p3) & 0xffff;
    v5     = imageCopy.at<uint16_t>(p6) & 0xffff;
    v8     = imageCopy.at<uint16_t>(p9) & 0xffff;
    if(roiX > 0) {
      p3++;
      p6++;
      p9++;
    }
    v3 = imageCopy.at<uint16_t>(p3) & 0xffff;
    v6 = imageCopy.at<uint16_t>(p6) & 0xffff;
    v9 = imageCopy.at<uint16_t>(p9) & 0xffff;

    for(int x = roiX; x < xEnd; x++, p++) {
      if(x < width - 1) {
        p3++;
        p6++;
        p9++;
      }
      v1            = v2;
      v2            = v3;
      v3            = imageCopy.at<uint16_t>(p3) & 0xffff;
      v4            = v5;
      v5            = v6;
      v6            = imageCopy.at<uint16_t>(p6) & 0xffff;
      v7            = v8;
      v8            = v9;
      v9            = imageCopy.at<uint16_t>(p9) & 0xffff;
      double sum1   = v1 + 2 * v2 + v3 - v7 - 2 * v8 - v9;
      double sum2   = v1 + 2 * v4 + v7 - v3 - 2 * v6 - v9;
      double result = std::sqrt(sum1 * sum1 + sum2 * sum2);
      if(result > 65535.0) {
        result = 65535.0;
      }
      image.at<uint16_t>(p) = static_cast<uint16_t>(result);
    }
  }
}

}    // namespace joda::cmd
