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
/// \class      ThresholdMoments
/// \author     Joachim Danmayr
/// \brief      W. Tsai, "Moment-preserving thresholding: a new approach," Computer Vision,
///             Graphics, and Image Processing, vol. 29, pp. 377-393, 1985.
///             Ported to ImageJ plugin by G.Landini from the the open source project FOURIER 0.8
///             by  M. Emre Celebi , Department of Computer Science,  Louisiana State University in Shreveport
///             Shreveport, LA 71115, USA
///              http://sourceforge.net/projects/fourier-ipal
///              http://www.lsus.edu/faculty/~ecelebi/fourier.htm
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdMoments final
{
public:
  static uint16_t calcThresholdValue(const cv::Mat &histogram)
  {
    double total = 0;
    double m0 = 1.0, m1 = 0.0, m2 = 0.0, m3 = 0.0, sum = 0.0, p0 = 0.0;
    double cd, c0, c1, z0, z1; /* auxiliary variables */
    int threshold = -1;

    double histo[256];

    for(int i = 0; i < 256; i++) {
      total += histogram.at<float>(i);
    }

    for(int i = 0; i < 256; i++) {
      histo[i] = (double) (histogram.at<float>(i) / total);    // normalised histogram
    }

    /* Calculate the first, second, and third order moments */
    for(int i = 0; i < 256; i++) {
      double di = i;
      m1 += di * histo[i];
      m2 += di * di * histo[i];
      m3 += di * di * di * histo[i];
    }
    /*
    First 4 moments of the gray-level image should match the first 4 moments
    of the target binary image. This leads to 4 equalities whose solutions
    are given in the Appendix of Ref. 1
    */
    cd = m0 * m2 - m1 * m1;
    c0 = (-m2 * m2 + m1 * m3) / cd;
    c1 = (m0 * -m3 + m2 * m1) / cd;
    z0 = 0.5 * (-c1 - std::sqrt(c1 * c1 - 4.0 * c0));
    z1 = 0.5 * (-c1 + std::sqrt(c1 * c1 - 4.0 * c0));
    p0 = (z1 - m1) / (z1 - z0); /* Fraction of the object pixels in the target binary image */

    // The threshold is the gray-level closest
    // to the p0-tile of the normalized histogram
    sum = 0;
    for(int i = 0; i < 256; i++) {
      sum += histo[i];
      if(sum > p0) {
        threshold = i;
        break;
      }
    }
    return threshold;
  }
};

}    // namespace joda::cmd
