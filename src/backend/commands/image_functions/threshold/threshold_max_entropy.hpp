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
/// \brief      Implements Kapur-Sahoo-Wong (Maximum Entropy) thresholding method
///             Kapur J.N., Sahoo P.K., and Wong A.K.C. (1985) "A New Method for
///             Gray-Level Picture Thresholding Using the Entropy of the Histogram"
///             Graphical Models and Image Processing, 29(3): 273-285
///             M. Emre Celebi
///             06.15.2007
///             Ported to ImageJ plugin by G.Landini from E Celebi's fourier_0.8 routines
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdMaxEntropy final
{
public:
  static uint16_t calcThresholdValue(const cv::Mat &histogram)
  {
    int threshold = -1;
    int ih, it;
    int first_bin;
    int last_bin;
    double tot_ent;         /* total entropy */
    double max_ent;         /* max entropy */
    double ent_back;        /* entropy of the background pixels at a given threshold */
    double ent_obj;         /* entropy of the object pixels at a given threshold */
    double norm_histo[256]; /* normalized histogram */
    double P1[256];         /* cumulative normalized histogram */
    double P2[256];

    double total = 0;
    for(ih = 0; ih < 256; ih++)
      total += histogram.at<float>(ih);

    for(ih = 0; ih < 256; ih++)
      norm_histo[ih] = histogram.at<float>(ih) / total;

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
    max_ent = std::numeric_limits<double>::min();

    for(it = first_bin; it <= last_bin; it++) {
      /* Entropy of the background pixels */
      ent_back = 0.0;
      for(ih = 0; ih <= it; ih++) {
        if(histogram.at<float>(ih) != 0) {
          ent_back -= (norm_histo[ih] / P1[it]) * std::log(norm_histo[ih] / P1[it]);
        }
      }

      /* Entropy of the object pixels */
      ent_obj = 0.0;
      for(ih = it + 1; ih < 256; ih++) {
        if(histogram.at<float>(ih) != 0) {
          ent_obj -= (norm_histo[ih] / P2[it]) * std::log(norm_histo[ih] / P2[it]);
        }
      }

      /* Total entropy */
      tot_ent = ent_back + ent_obj;

      // IJ.log(""+max_ent+"  "+tot_ent);
      if(max_ent < tot_ent) {
        max_ent   = tot_ent;
        threshold = it;
      }
    }
    return threshold;
  }
};

}    // namespace joda::cmd
