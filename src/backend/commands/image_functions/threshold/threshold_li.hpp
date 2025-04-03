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
/// \class      ThresholdTriangle
/// \author     Joachim Danmayr
/// \brief      Implements Li's Minimum Cross Entropy thresholding method
///             This implementation is based on the iterative version (Ref. 2) of the algorithm.
///             1) Li C.H. and Lee C.K. (1993) "Minimum Cross Entropy Thresholding"
///                Pattern Recognition, 26(4): 617-625
///             2) Li C.H. and Tam P.K.S. (1998) "An Iterative Algorithm for Minimum
///                Cross Entropy Thresholding"Pattern Recognition Letters, 18(8): 771-776
///             3) Sezgin M. and Sankur B. (2004) "Survey over Image Thresholding
///                Techniques and Quantitative Performance Evaluation" Journal of
///                Electronic Imaging, 13(1): 146-165
///                http://citeseer.ist.psu.edu/sezgin04survey.html
///             Ported to ImageJ plugin by G.Landini from E Celebi's fourier_0.8 routines
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdLi final
{
public:
  static uint16_t calcThresholdValue(cv::Mat &histogram)
  {
    // LI algorithm
    int threshold;
    double sum_back;    // sum of the background pixels at a given threshold
    double sum_obj;     // sum of the object pixels at a given threshold
    double num_back;    // number of background pixels at a given threshold
    double num_obj;     // number of object pixels at a given threshold
    double old_thresh;
    double mean_back;    // mean of the background pixels at a given threshold
    double mean_obj;     // mean of the object pixels at a given threshold
    double temp;

    double tolerance  = 0.5;    // threshold tolerance
    double num_pixels = 0;
    for(int ih = 0; ih < histogram.total(); ih++) {
      num_pixels += histogram.at<float>(ih);
    }

    /* Calculate the mean gray-level */
    double mean = 0.0;
    for(int ih = 0 + 1; ih < histogram.total(); ih++) {    // 0 + 1?
      mean += static_cast<double>(ih) * histogram.at<float>(ih);
    }
    mean /= num_pixels;
    /* Initial estimate */
    double new_thresh = mean;

    do {
      old_thresh = new_thresh;
      threshold  = static_cast<int>(old_thresh + 0.5); /* range */
      /* Calculate the means of background and object pixels */
      /* Background */
      sum_back = 0;
      num_back = 0;
      for(int ih = 0; ih <= threshold; ih++) {
        sum_back += static_cast<double>(ih) * histogram.at<float>(ih);
        num_back += histogram.at<float>(ih);
      }
      mean_back = (num_back == 0 ? 0.0 : (sum_back / num_back));
      /* Object */
      sum_obj = 0;
      num_obj = 0;
      for(int ih = threshold + 1; ih < histogram.total(); ih++) {
        sum_obj += static_cast<double>(ih) * histogram.at<float>(ih);
        num_obj += histogram.at<float>(ih);
      }
      mean_obj = (num_obj == 0 ? 0.0 : (sum_obj / num_obj));

      /* Calculate the new threshold: Equation (7) in Ref. 2 */
      // new_thresh = simple_round ( ( mean_back - mean_obj ) / ( Math.log ( mean_back ) - Math.log ( mean_obj ) ) );
      // simple_round ( double x ) {
      //  return ( int ) ( IS_NEG ( x ) ? x - .5 : x + .5 );
      // }
      //
      // #define IS_NEG( x ) ( ( x ) < -DBL_EPSILON )
      // DBL_EPSILON = 2.220446049250313E-16
      temp = (mean_back - mean_obj) / (std::log(mean_back) - std::log(mean_obj));

      if(temp < -2.220446049250313E-16) {
        new_thresh = static_cast<int>(temp - 0.5);
      } else {
        new_thresh = static_cast<int>(temp + 0.5);
      }
      /*  Stop the iterations when the difference between the
      new and old threshold values is less than the tolerance */
    } while(std::abs(new_thresh - old_thresh) > tolerance);

    return threshold;
  }
};

}    // namespace joda::cmd
