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
/// \brief     A short description what happens here.
///

#pragma once

#include <opencv2/core/hal/interface.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core/mat.hpp>
#include "threshold.hpp"

namespace joda::image::func {

///
/// \class      ThresholdMinError
/// \author     Joachim Danmayr
/// \brief       Kittler and J. Illingworth, "Minimum error thresholding," Pattern Recognition, vol. 19, pp. 41-47,
/// 1986.
///              C. A. Glasbey, "An analysis of histogram-based thresholding algorithms," CVGIP: Graphical Models and
///              Image Processing, vol. 55, pp. 532-537, 1993. Ported to ImageJ plugin by G.Landini from Antti
///              Niemisto's Matlab code (GPL) Original Matlab code Copyright (C) 2004 Antti Niemisto See
///              http://www.cs.tut.fi/~ant/histthresh/ for an excellent slide presentation and the original Matlab code.
///
/// \ref       Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdMinError final : public Threshold
{
public:
  using Threshold::Threshold;

private:
  [[nodiscard]] uint16_t calcThresholdValue(cv::Mat &histogram) const
  {
    auto mean = [](const cv::Mat &data) -> float {
      // C. A. Glasbey, "An analysis of histogram-based thresholding algorithms,"
      // CVGIP: Graphical Models and Image Processing, vol. 55, pp. 532-537, 1993.
      //
      // The threshold is the mean of the greyscale data
      float threshold = -1;
      double tot      = 0;
      double sum      = 0;
      for(int i = 0; i < data.total(); i++) {
        tot += data.at<float>(i);
        sum += ((double) i * (double) data.at<float>(i));
      }
      threshold = std::floor(sum / tot);
      return threshold;
    };

    auto A = [](const cv::Mat &y, int j) -> double {
      double x = 0;
      for(int i = 0; i <= j; i++)
        x += y.at<float>(i);
      return x;
    };

    auto B = [](const cv::Mat &y, int j) -> double {
      double x = 0;
      for(int i = 0; i <= j; i++)
        x += i * y.at<float>(i);
      return x;
    };

    auto C = [](const cv::Mat &y, int j) -> double {
      double x = 0;
      for(int i = 0; i <= j; i++)
        x += i * i * y.at<float>(i);
      return x;
    };

    // Calc
    float threshold = mean(histogram);    // Initial estimate for the threshold is found with the MEAN algorithm.
    int Tprev       = -2;
    double mu;
    double nu;
    double p;
    double q;
    double sigma2;
    double tau2;
    double w0;
    double w1;
    double w2;
    double sqterm;
    double temp;
    // int counter=1;
    int32_t histoLength = histogram.total();
    while(threshold != Tprev) {
      // Calculate some statistics.
      mu = B(histogram, threshold) / A(histogram, threshold);
      nu = (B(histogram, histoLength - 1) - B(histogram, threshold)) /
           (A(histogram, histoLength - 1) - A(histogram, threshold));
      p      = A(histogram, threshold) / A(histogram, histoLength - 1);
      q      = (A(histogram, histoLength - 1) - A(histogram, threshold)) / A(histogram, histoLength - 1);
      sigma2 = C(histogram, threshold) / A(histogram, threshold) - (mu * mu);
      tau2   = (C(histogram, histoLength - 1) - C(histogram, threshold)) /
                 (A(histogram, histoLength - 1) - A(histogram, threshold)) -
             (nu * nu);

      // The terms of the quadratic equation to be solved.
      w0 = 1.0 / sigma2 - 1.0 / tau2;
      w1 = mu / sigma2 - nu / tau2;
      w2 = (mu * mu) / sigma2 - (nu * nu) / tau2 + std::log10((sigma2 * (q * q)) / (tau2 * (p * p)));

      // If the next threshold would be imaginary, return with the current one.
      sqterm = (w1 * w1) - w0 * w2;
      if(sqterm < 0) {
        joda::log::logWarning("MinError(I): not converging.");
        return threshold;
      }

      // The updated threshold is the integer part of the solution of the quadratic equation.
      Tprev = threshold;
      temp  = (w1 + std::sqrt(sqterm)) / w0;

      if(temp != temp) {
        threshold = Tprev;
      } else {
        threshold = static_cast<int>(std::floor(temp));
      }
    }

    return threshold;
  }
};
}    // namespace joda::image::func
