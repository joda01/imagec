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

#include <opencv2/core/hal/interface.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::cmd {

///
/// \class      ThresholdOtsu
/// \author     Joachim Danmayr
/// \brief      C++ code by Jordan Bevik <Jordan.Bevic@qtiworld.com>
///             ported to ImageJ plugin by G.Landini
///             and again back to C++ by Joachim Danmayr
///
/// \ref       Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdOtsu final
{
public:
  static uint16_t calcThresholdValue(cv::Mat &histogram)
  {
    int k         = 0;
    int kStar     = 0;    // k = the current threshold; kStar = optimal threshold
    double N1     = 0;
    double N      = 0;    // N1 = # points with intensity <=k; N = total number of points
    double BCV    = 0;
    double BCVmax = 0;    // The current Between Class Variance and maximum BCV
    double num    = 0;
    double denom  = 0;    // temporary bookeeping
    double Sk     = 0;    // The total intensity for all histogram points <=k
    double S      = 0;
    double L      = 256;    // The total intensity of the image

    // Initialize values:
    S = N = 0;
    for(k = 0; k < L; k++) {
      S += static_cast<double>(k) * histogram.at<float>(k);    // Total histogram intensity
      N += histogram.at<float>(k);                             // Total number of data points
    }

    Sk     = 0;
    N1     = histogram.at<float>(0);    // The entry for zero intensity
    BCV    = 0;
    BCVmax = 0;
    kStar  = 0;

    // Look at each possible threshold value,
    // calculate the between-class variance, and decide if it's a max
    for(k = 1; k < L - 1; k++) {    // No need to check endpoints k = 0 or k = L-1
      Sk += static_cast<double>(k) * histogram.at<float>(k);
      N1 += histogram.at<float>(k);

      // The float casting here is to avoid compiler warning about loss of precision and
      // will prevent overflow in the case of large saturated images
      denom = (N1) * (N - N1);    // Maximum value of denom is (N^2)/4 =  approx. 3E10

      if(denom != 0) {
        // Float here is to avoid loss of precision when dividing
        num = (N1 / N) * S - Sk;    // Maximum value of num =  255*N = approx 8E7
        BCV = (num * num) / denom;
      } else {
        BCV = 0;
      }

      if(BCV >= BCVmax) {    // Assign the best threshold found so far
        BCVmax = BCV;
        kStar  = k;
      }
    }
    // kStar += 1;  // Use QTI convention that intensity -> 1 if intensity >= k
    // (the algorithm was developed for I-> 1 if I <= k.)
    return kStar;
  }
};

}    // namespace joda::cmd
