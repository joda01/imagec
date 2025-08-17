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
///             j and k
///             Threshold t is (j+k)/2.
///             Images with histograms having extremely unequal peaks or a broad and
///             flat valleys are unsuitable for this method.
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdIntermodes final
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
    int minbin = -1;
    int maxbin = -1;
    for(int i = 0; i < 256; i++) {
      if(histogram.at<float>(i) > 0) {
        maxbin = i;
      }
    }
    for(int i = 256 - 1; i >= 0; i--) {
      if(histogram.at<float>(i) > 0) {
        minbin = i;
      }
    }
    int length = (maxbin - minbin) + 1;
    auto *hist = new double[length];
    for(int i = minbin; i <= maxbin; i++) {
      hist[i - minbin] = static_cast<double>(histogram.at<float>(i));
    }

    int iter      = 0;
    int threshold = -1;
    while(!bimodalTest(hist, length)) {
      // smooth with a 3 point running mean filter
      double previous = 0;
      double current  = 0;
      double next     = hist[0];
      for(int i = 0; i < length - 1; i++) {
        previous = current;
        current  = next;
        next     = hist[i + 1];
        hist[i]  = (previous + current + next) / 3;
      }
      hist[length - 1] = (current + next) / 3;
      iter++;
      if(iter > 10000) {
        threshold = -1;
        // IJ.log("Intermodes Threshold not found after 10000 iterations.");
        delete[] hist;
        return static_cast<uint16_t>(threshold);
      }
    }

    // The threshold is the mean between the two peaks.
    int tt = 0;
    for(int i = 1; i < length - 1; i++) {
      if(hist[i - 1] < hist[i] && hist[i + 1] < hist[i]) {
        tt += i;
        // IJ.log("mode:" +i);
      }
    }
    threshold = static_cast<int>(std::floor(tt / 2.0));
    delete[] hist;
    return static_cast<uint16_t>(threshold + minbin);
  }
};

}    // namespace joda::cmd
