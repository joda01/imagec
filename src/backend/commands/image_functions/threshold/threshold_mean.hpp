///
/// \file      threshold.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <algorithm>
#include <cstdint>
#include <opencv2/core/mat.hpp>

namespace joda::cmd {

///
/// \class      ThresholdMean
/// \author     Joachim Danmayr
/// \brief      C. A. Glasbey, "An analysis of histogram-based thresholding algorithms,"
///             CVGIP: Graphical Models and Image Processing, vol. 55, pp. 532-537, 1993.
///             The threshold is the mean of the greyscale data
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdMean final
{
public:
  static uint16_t calcThresholdValue(const cv::Mat &histogram)
  {
    int threshold = -1;
    double tot    = 0;
    double sum    = 0;
    for(int i = 0; i < 256; i++) {
      tot += histogram.at<float>(i);
      sum += (static_cast<double>(i) * histogram.at<float>(i));
    }
    threshold = static_cast<int>(std::floor(sum / tot));
    return threshold;
  }
};

}    // namespace joda::cmd
