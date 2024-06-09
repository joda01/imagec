///
/// \file      postprocessing.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "postprocessing.hpp"
#include <opencv2/opencv.hpp>

namespace joda::image::postprocessing {

cv::Mat PostProcessor::markPositionInImage(const cv::Mat &image, int x, int y)
{
  // Point of interest (change this to your desired point)
  cv::Point pointOfInterest(x, y);    // Example: center of the image

  // Draw horizontal line
  cv::line(image, cv::Point(0, pointOfInterest.y), cv::Point(image.cols - 1, pointOfInterest.y),
           cv::Scalar(0, 0, 255, 128), 2);

  // Draw vertical line
  cv::line(image, cv::Point(pointOfInterest.x, 0), cv::Point(pointOfInterest.x, image.rows - 1),
           cv::Scalar(0, 0, 255, 128), 2);

  return image;
}

}    // namespace joda::image::postprocessing
