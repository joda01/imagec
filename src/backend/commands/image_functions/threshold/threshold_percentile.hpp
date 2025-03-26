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
/// \brief      W. Doyle, "Operation useful for similarity-invariant pattern recognition,"
///             Journal of the Association for Computing Machinery, vol. 9,pp. 259-267, 1962.
///             ported to ImageJ plugin by G.Landini from Antti Niemisto's Matlab code (GPL)
///             Original Matlab code Copyright (C) 2004 Antti Niemisto
///             See http://www.cs.tut.fi/~ant/histthresh/ for an excellent slide presentation
///             and the original Matlab code.
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdPercentile final
{
public:
  static uint16_t calcThresholdValue(const cv::Mat &histogram)
  {
    int iter      = 0;
    int threshold = -1;
    double ptile  = 0.5;    // default fraction of foreground pixels
    double avec[256];

    for(int i = 0; i < 256; i++)
      avec[i] = 0.0;

    double total = partialSum(histogram, 255);
    double temp  = 1.0;
    for(int i = 0; i < 256; i++) {
      avec[i] = std::abs((partialSum(histogram, i) / total) - ptile);
      // IJ.log("Ptile["+i+"]:"+ avec[i]);
      if(avec[i] < temp) {
        temp      = avec[i];
        threshold = i;
      }
    }
    return threshold;
  }

private:
  static double partialSum(const cv::Mat &y, int j)
  {
    double x = 0;
    for(int i = 0; i <= j; i++) {
      x += y.at<float>(i);
    }
    return x;
  }
};

}    // namespace joda::cmd
