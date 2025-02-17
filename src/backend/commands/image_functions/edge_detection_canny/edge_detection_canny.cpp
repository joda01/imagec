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

#include "edge_detection_canny.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

void EdgeDetectionCanny::canny(cv::Mat &image) const
{
  cv::Mat binaryImage(image.size(), CV_8UC1);
  image.convertTo(binaryImage, CV_8UC1, 255.0 / 65535.0);
  cv::Canny(binaryImage, binaryImage, mSetting.thresholdMin, mSetting.thresholdMax, mSetting.kernelSize);
  binaryImage.convertTo(image, CV_16UC1, 257.0);
}

}    // namespace joda::cmd
