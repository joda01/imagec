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
/// \brief      Zack, G. W., Rogers, W. E. and Latt, S. A., 1977,
///             Automatic Measurement of Sister Chromatid Exchange Frequency,
///             Journal of Histochemistry and Cytochemistry 25 (7), pp. 741-753
///             modified from Johannes Schindelin plugin
///
/// \ref        Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdTriangle final
{
public:
  static uint16_t calcThresholdValue(cv::Mat &histogram)
  {
    // find min and max
    int min  = 0;
    int dmax = 0;
    int max  = 0;
    int min2 = 0;
    for(int i = 0; i < static_cast<int>(histogram.total()); i++) {
      if(histogram.at<float>(i) > 0) {
        min = i;
        break;
      }
    }
    if(min > 0) {
      min--;    // line to the (p==0) point, not to data[min]
    }

    // The Triangle algorithm cannot tell whether the data is skewed to one side or another.
    // This causes a problem as there are 2 possible thresholds between the max and the 2 extremes
    // of the histogram.
    // Here I propose to find out to which side of the max point the data is furthest, and use that as
    //  the other extreme.
    for(int i = 255; i > 0; i--) {
      if(histogram.at<float>(i) > 0) {
        min2 = i;
        break;
      }
    }
    if(min2 < 255) {
      min2++;    // line to the (p==0) point, not to data[min]
    }

    for(int i = 0; i < 256; i++) {
      if(histogram.at<float>(i) > static_cast<float>(dmax)) {
        max  = i;
        dmax = static_cast<int>(histogram.at<float>(i));
      }
    }
    // find which is the furthest side
    // IJ.log(""+min+" "+max+" "+min2);
    bool inverted = false;
    if((max - min) < (min2 - max)) {
      // reverse the histogram
      // IJ.log("Reversing histogram.");
      inverted  = true;
      int left  = 0;      // index of leftmost element
      int right = 255;    // index of rightmost element
      while(left < right) {
        // exchange the left and right elements
        float temp                 = histogram.at<float>(left);
        histogram.at<float>(left)  = histogram.at<float>(right);
        histogram.at<float>(right) = temp;
        // move the bounds toward the center
        left++;
        right--;
      }
      min = 255 - min2;
      max = 255 - max;
    }

    if(min == max) {
      // IJ.log("Triangle:  min == max.");
      return static_cast<uint16_t>(min);
    }

    // describe line by nx * x + ny * y - d = 0
    double nx;
    double ny;
    double d;
    // nx is just the max frequency as the other point has freq=0
    nx = static_cast<double>(histogram.at<float>(max));    //-min; // data[min]; //  lowest value bmin = (p=0)% in the image
    ny = min - max;
    d  = std::sqrt(nx * nx + ny * ny);
    nx /= d;
    ny /= d;
    d = nx * min + ny * static_cast<double>(histogram.at<float>(min));

    // find split point
    int split            = min;
    double splitDistance = 0;
    for(int i = min + 1; i <= max; i++) {
      double newDistance = nx * i + ny * static_cast<double>(histogram.at<float>(i)) - d;
      if(newDistance > splitDistance) {
        split         = i;
        splitDistance = newDistance;
      }
    }
    split--;

    if(inverted) {
      // The histogram might be used for something else, so let's reverse it back
      int left  = 0;
      int right = 255;
      while(left < right) {
        float temp                 = histogram.at<float>(left);
        histogram.at<float>(left)  = histogram.at<float>(right);
        histogram.at<float>(right) = temp;
        left++;
        right--;
      }
      return static_cast<uint16_t>(255 - split);
    }
    return static_cast<uint16_t>(split);
  }
};

}    // namespace joda::cmd
