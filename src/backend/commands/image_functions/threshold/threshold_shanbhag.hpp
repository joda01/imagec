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
/// \brief      C. A. Glasbey, "An analysis of histogram-based thresholding algorithms,"
///             CVGIP: Graphical Models and Image Processing, vol. 55, pp. 532-537, 1993.
///             The threshold is the mean of the greyscale data
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdShanbhag final
{
public:
  static uint16_t calcThresholdValue(const cv::Mat &histogram)
  {
    // Shanhbag A.G. (1994) "Utilization of Information Measure as a Means of
    //  Image Thresholding" Graphical Models and Image Processing, 56(5): 414-419
    // Ported to ImageJ plugin by G.Landini from E Celebi's fourier_0.8 routines
    int threshold;
    int ih;
    int it;
    int first_bin;
    int last_bin;
    double term;
    double tot_ent;         /* total entropy */
    double min_ent;         /* max entropy */
    double ent_back;        /* entropy of the background pixels at a given threshold */
    double ent_obj;         /* entropy of the object pixels at a given threshold */
    double norm_histo[256]; /* normalized histogram */
    double P1[256];         /* cumulative normalized histogram */
    double P2[256];

    double total = 0;
    for(ih = 0; ih < 256; ih++) {
      total += static_cast<double>(histogram.at<float>(ih));
    }

    for(ih = 0; ih < 256; ih++) {
      norm_histo[ih] = static_cast<double>(histogram.at<float>(ih)) / total;
    }

    P1[0] = norm_histo[0];
    P2[0] = 1.0 - P1[0];
    for(ih = 1; ih < 256; ih++) {
      P1[ih] = P1[ih - 1] + norm_histo[ih];
      P2[ih] = 1.0 - P1[ih];
    }

    /* Determine the first non-zero bin */
    first_bin = 0;
    for(ih = 0; ih < 256; ih++) {
      if(!(std::abs(P1[ih]) < 2.220446049250313E-16)) {
        first_bin = ih;
        break;
      }
    }

    /* Determine the last non-zero bin */
    last_bin = 255;
    for(ih = 255; ih >= first_bin; ih--) {
      if(!(std::abs(P2[ih]) < 2.220446049250313E-16)) {
        last_bin = ih;
        break;
      }
    }

    // Calculate the total entropy each gray-level
    // and find the threshold that maximizes it
    threshold = -1;
    min_ent   = std::numeric_limits<double>::max();

    for(it = first_bin; it <= last_bin; it++) {
      /* Entropy of the background pixels */
      ent_back = 0.0;
      term     = 0.5 / P1[it];
      for(ih = 1; ih <= it; ih++) {    // 0+1?
        ent_back -= norm_histo[ih] * std::log(1.0 - term * P1[ih - 1]);
      }
      ent_back *= term;

      /* Entropy of the object pixels */
      ent_obj = 0.0;
      term    = 0.5 / P2[it];
      for(ih = it + 1; ih < 256; ih++) {
        ent_obj -= norm_histo[ih] * std::log(1.0 - term * P2[ih]);
      }
      ent_obj *= term;

      /* Total entropy */
      tot_ent = std::abs(ent_back - ent_obj);

      if(tot_ent < min_ent) {
        min_ent   = tot_ent;
        threshold = it;
      }
    }
    return static_cast<uint16_t>(threshold);
  }
};

}    // namespace joda::cmd
