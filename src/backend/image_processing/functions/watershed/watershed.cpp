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

  // dist_transform = cv::Scalar::all(255) - dist_transform;
  // cv::Mat erodes;
  // cv::erode(dist_transform, erodes, cv::Mat{}, cv::Point(-1, -1), 3);
  // cv::imwrite("zz_sdist_erodes.jpg", erodes);

  cv::morphologyEx(thresholded, thresholded, cv::MORPH_OPEN, cv::Mat{});

  cv::Mat dilates;
  cv::dilate(thresholded, dilates, cv::Mat{});
  cv::imwrite("zz_sdist_dilates.jpg", dilates);

  cv::Mat dist_transform;
  cv::distanceTransform(thresholded, dist_transform, cv::DIST_L2, 0);    // 5
  cv::imwrite("zz_sdist_transform.jpg", dist_transform * 100);
  cv::Mat foreground;
  cv::threshold(dist_transform, foreground, 0.7 * getMax(dist_transform), 255, 0);
  cv::imwrite("zz_sdist_transform_points.jpg", foreground);
  foreground.convertTo(foreground, CV_8U);

  cv::Mat unknown;
  cv::subtract(dilates, foreground, unknown);
  cv::imwrite("zz_sdist_unknown.jpg", unknown);

  cv::Mat markers;
  cv::connectedComponents(foreground, markers);
  markers = markers + 1;    // 1
  cv::imwrite("zz_sdist_markers50.jpg", markers * 100);

  // Find the location of pixels in 'unknown' with value 255
  cv::Mat unknown_mask = (unknown == 255);
  cv::imwrite("zz_sdist_unknown_mask.jpg", unknown_mask);
  markers.setTo(0, unknown_mask);

  cv::imwrite("zz_sdist_markers.jpg", markers * 50);

  cv::Mat mask;
  markers.convertTo(mask, CV_32SC1);
  cv::watershed(inputImage, mask);

  cv::imwrite("zz_sdist_watersehd.jpg", mask);

  cv::Mat watersheedMask = (mask == -1);
  cv::Mat outMask        = cv::Mat::ones(mask.rows, mask.cols, CV_16UC1) * 65535;
  outMask.setTo(0, watersheedMask);

  cv::imwrite("zz_sdist_watersehd_mask.jpg", outMask);

  cv::bitwise_and(image, outMask, image);
  cv::imwrite("zz_sdist_watersehd_mask_out.jpg", image);
}

}    // namespace joda::func::img
     // https://docs.opencv.org/4.x/d3/db4/tutorial_py_watershed.html
