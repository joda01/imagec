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
/// \brief      J. M. S. Prewitt and M. L. Mendelsohn, "The analysis of cell images," in
///             Annals of the New York Academy of Sciences, vol. 128, pp. 1035-1053, 1966.
///             ported to ImageJ plugin by G.Landini from Antti Niemisto's Matlab code (GPL)
///             Original Matlab code Copyright (C) 2004 Antti Niemisto
///             See http://www.cs.tut.fi/~ant/histthresh/ for an excellent slide presentation
///             and the original Matlab code.
///
///             Assumes a bimodal histogram. The histogram needs is smoothed (using a
///             running average of size 3, iteratively) until there are only two local maxima.
///             Threshold t is such that yt-1 > yt <= yt+1.
///             Images with histograms having extremely unequal peaks or a broad and
///             flat valleys are unsuitable for this method.
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdMinimum final
{
public:
  static bool bimodalTest(const double *y, int len)
  {
    bool b    = false;
    int modes = 0;

    for(int k = 1; k < len - 1; k++) {
      if(y[k - 1] < y[k] && y[k + 1] < y[k]) {
        modes++;
        if(modes > 2) {
          return false;
        }
      }
    }
    if(modes == 2) {
      b = true;
    }
    return b;
  }

  static uint16_t calcThresholdValue(const cv::Mat &histogram)
  {
    int iter      = 0;
    int threshold = -1;
    double iHisto[256];
    for(int i = 0; i < 256; i++) {
      iHisto[i] = (double) histogram.at<float>(i);
    }
    double tHisto[256];

    while(!bimodalTest(iHisto, 256)) {
      // smooth with a 3 point running mean filter
      for(int i = 1; i < 255; i++) {
        tHisto[i] = (iHisto[i - 1] + iHisto[i] + iHisto[i + 1]) / 3;
      }
      tHisto[0]   = (iHisto[0] + iHisto[1]) / 3;        // 0 outside
      tHisto[255] = (iHisto[254] + iHisto[255]) / 3;    // 0 outside
      std::copy(tHisto, tHisto + 256, iHisto);
      iter++;
      if(iter > 10000) {
        threshold = -1;
        // IJ.log("Minimum: threshold not found after 10000 iterations.");
        return threshold;
      }
    }
    // The threshold is the minimum between the two peaks.
    for(int i = 1; i < 255; i++) {
      if(iHisto[i - 1] > iHisto[i] && iHisto[i + 1] >= iHisto[i]) {
        threshold = i;
        break;
      }
    }
    return threshold;
  }
};

}    // namespace joda::cmd
