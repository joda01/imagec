///
/// \file      threshold.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

namespace joda::cmd {

///
/// \class      ThresholdImageMean
/// \author     Joachim Danmayr
/// \brief      Calculates the mean average of the image an doubles the value
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdImageMean final
{
public:
  static uint16_t calcThresholdValue(const cv::Mat &srcImg)
  {
    double min;
    double max;
    cv::Point min_loc;
    cv::Point max_loc;
    // Find the minimum and maximum values and their locations
    cv::minMaxLoc(srcImg, &min, &max, &min_loc, &max_loc);

    // Calculate mean
    cv::Mat mean;
    cv::Mat stddev;
    cv::meanStdDev(srcImg, mean, stddev);

    double m = mean.at<double>(0);
    double s = stddev.at<double>(0);

    std::cout << "STDEV: " << std::to_string(s) << " Mean: " << std::to_string(m) << std::endl;
    std::cout << "MIN: " << std::to_string(min) << " MAX: " << std::to_string(max) << std::endl;

    //$$WANT \approx (7.08 \times MEAN) - (2.73 \times STD) + 180$$

    double want = (7.08 * m) - (2.73 * s) + 180;

    std::cout << "WANT: " << std::to_string(want) << std::endl;

    return static_cast<uint16_t>(want);
  }
};

}    // namespace joda::cmd
