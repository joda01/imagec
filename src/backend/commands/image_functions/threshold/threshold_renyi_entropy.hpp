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
/// \brief       Kapur J.N., Sahoo P.K., and Wong A.K.C. (1985) "A New Method for
///              Gray-Level Picture Thresholding Using the Entropy of the Histogram"
///              Graphical Models and Image Processing, 29(3): 273-285
///              M. Emre Celebi
///              06.15.2007
///              Ported to ImageJ plugin by G.Landini from E Celebi's fourier_0.8 routines
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdRenyiEntropy final
{
public:
  static uint16_t calcThresholdValue(const cv::Mat &histogram)
  {
    int threshold;
    int opt_threshold;

    int ih, it;
    int first_bin;
    int last_bin;
    int tmp_var;
    int t_star1, t_star2, t_star3;
    int beta1, beta2, beta3;
    double alpha; /* alpha parameter of the method */
    double term;
    double tot_ent;  /* total entropy */
    double max_ent;  /* max entropy */
    double ent_back; /* entropy of the background pixels at a given threshold */
    double ent_obj;  /* entropy of the object pixels at a given threshold */
    double omega;
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

    /* Maximum Entropy Thresholding - BEGIN */
    /* ALPHA = 1.0 */
    /* Calculate the total entropy each gray-level
    and find the threshold that maximizes it
    */
    threshold = 0;    // was MIN_INT in original code, but if an empty image is processed it gives an error later on.
    max_ent   = 0.0;

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
    t_star2 = threshold;

    /* Maximum Entropy Thresholding - END */
    threshold = 0;    // was MIN_INT in original code, but if an empty image is processed it gives an error later on.
    max_ent   = 0.0;
    alpha     = 0.5;
    term      = 1.0 / (1.0 - alpha);
    for(it = first_bin; it <= last_bin; it++) {
      /* Entropy of the background pixels */
      ent_back = 0.0;
      for(ih = 0; ih <= it; ih++)
        ent_back += std::sqrt(norm_histo[ih] / P1[it]);

      /* Entropy of the object pixels */
      ent_obj = 0.0;
      for(ih = it + 1; ih < 256; ih++)
        ent_obj += std::sqrt(norm_histo[ih] / P2[it]);

      /* Total entropy */
      tot_ent = term * ((ent_back * ent_obj) > 0.0 ? std::log(ent_back * ent_obj) : 0.0);

      if(tot_ent > max_ent) {
        max_ent   = tot_ent;
        threshold = it;
      }
    }

    t_star1 = threshold;

    threshold = 0;    // was MIN_INT in original code, but if an empty image is processed it gives an error later on.
    max_ent   = 0.0;
    alpha     = 2.0;
    term      = 1.0 / (1.0 - alpha);
    for(it = first_bin; it <= last_bin; it++) {
      /* Entropy of the background pixels */
      ent_back = 0.0;
      for(ih = 0; ih <= it; ih++)
        ent_back += (norm_histo[ih] * norm_histo[ih]) / (P1[it] * P1[it]);

      /* Entropy of the object pixels */
      ent_obj = 0.0;
      for(ih = it + 1; ih < 256; ih++)
        ent_obj += (norm_histo[ih] * norm_histo[ih]) / (P2[it] * P2[it]);

      /* Total entropy */
      tot_ent = term * ((ent_back * ent_obj) > 0.0 ? std::log(ent_back * ent_obj) : 0.0);

      if(tot_ent > max_ent) {
        max_ent   = tot_ent;
        threshold = it;
      }
    }

    t_star3 = threshold;

    /* Sort t_star values */
    if(t_star2 < t_star1) {
      tmp_var = t_star1;
      t_star1 = t_star2;
      t_star2 = tmp_var;
    }
    if(t_star3 < t_star2) {
      tmp_var = t_star2;
      t_star2 = t_star3;
      t_star3 = tmp_var;
    }
    if(t_star2 < t_star1) {
      tmp_var = t_star1;
      t_star1 = t_star2;
      t_star2 = tmp_var;
    }

    /* Adjust beta values */
    if(std::abs(t_star1 - t_star2) <= 5) {
      if(std::abs(t_star2 - t_star3) <= 5) {
        beta1 = 1;
        beta2 = 2;
        beta3 = 1;
      } else {
        beta1 = 0;
        beta2 = 1;
        beta3 = 3;
      }
    } else {
      if(std::abs(t_star2 - t_star3) <= 5) {
        beta1 = 3;
        beta2 = 1;
        beta3 = 0;
      } else {
        beta1 = 1;
        beta2 = 2;
        beta3 = 1;
      }
    }
    // IJ.log(""+t_star1+" "+t_star2+" "+t_star3);
    /* Determine the optimal threshold value */
    omega = P1[t_star3] - P1[t_star1];
    opt_threshold =
        (int) (t_star1 * (P1[t_star1] + 0.25 * omega * beta1) + 0.25 * t_star2 * omega * beta2 + t_star3 * (P2[t_star3] + 0.25 * omega * beta3));

    return opt_threshold;
  }
};

}    // namespace joda::cmd
