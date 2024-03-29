///
/// \file      watershed.cpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "watershed.hpp"
#include <opencv2/core/hal/interface.h>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::func::img {

float getMax(const cv::Mat &mat)
{
  double minVal, maxVal;
  cv::Point minLoc, maxLoc;

  // Find minimum and maximum pixel values and their locations
  cv::minMaxLoc(mat, &minVal, &maxVal, &minLoc, &maxLoc);

  return maxVal;
}

/*

Watershed segmentation is a way of automatically separating or cutting apart particles that touch. It first calculates
the Euclidian distance map (EDM) and finds the ultimate eroded points (UEPs). It then dilates each of the UEPs (the
peaks or local maxima of the EDM) as far as possible - either until the edge of the particle is reached, or the edge of
the region of another (growing) UEP. Watershed segmentation works best for smooth convex objects that don't overlap too
much.
*/

void Watershed::execute(cv::Mat &image) const
{
  cv::imwrite("zz__a_input_image.jpg", image);

  cv::Mat grayImageFloat;
  image.convertTo(grayImageFloat, CV_8UC3, (float) UCHAR_MAX / (float) UINT16_MAX);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);

  cv::Mat thresholded;
  image.convertTo(thresholded, CV_8U, 1.0F / 257.0F);

  /////////////////////////////////////////////////////////////////////////////////////
  cv::Mat distTransform;
  // thresholded = cv::Scalar::all(255) - thresholded;
  cv::distanceTransform(thresholded, distTransform, cv::DIST_L2, 0);
  cv::imwrite("zz_distTransform.jpg", distTransform);

  cv::Mat localMax;
  // find pixels that are equal to the local neighborhood not maximum (including 'plateaus')
  cv::dilate(distTransform, localMax, cv::Mat());
  cv::compare(distTransform, localMax, localMax, cv::CMP_GE);

  // optionally filter out pixels that are equal to the local minimum ('plateaus')
  if(true) {
    cv::Mat non_plateau_mask;
    cv::erode(image, non_plateau_mask, cv::Mat());
    cv::compare(image, non_plateau_mask, non_plateau_mask, cv::CMP_GT);
    cv::bitwise_and(localMax, non_plateau_mask, localMax);
  }

  cv::imwrite("zz_localMAx.jpg", localMax);

  cv::Mat markers;
  cv::connectedComponents(localMax, markers);
  markers = markers + 1;    // 1

  cv::imwrite("zz_sdist_wateshed_markers.jpg", markers);

  localMax.convertTo(markers, CV_32SC1);
  cv::watershed(inputImage, markers);
  cv::imwrite("zz_sdist_wateshed.jpg", markers);

  cv::Mat watersheedMask = (markers == -1);
  cv::Mat outMask        = cv::Mat::ones(markers.rows, markers.cols, CV_16UC1) * 65535;
  outMask.setTo(0, watersheedMask);
  cv::imwrite("zz_sdist_watersheedMask.jpg", outMask);

  cv::bitwise_and(outMask, image, image);

  // distTransform = cv::Scalar::all(255) - dist_transform;
  // cv::Mat erodes;
  // cv::erode(dist_transform, erodes, cv::Mat{}, cv::Point(-1, -1), 3);
  cv::imwrite("zz_and.jpg", image);
}

}    // namespace joda::func::img
     // https://docs.opencv.org/4.x/d3/db4/tutorial_py_watershed.html
     // https://github.com/imagej/ImageJ/blob/79714089e88fbc83a1499126b20ef5bb04fd3a29/ij/plugin/filter/EDM.java#L188
