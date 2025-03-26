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

#pragma once

#include <algorithm>
#include <cstdint>
#include <opencv2/core/mat.hpp>

namespace joda::cmd {

///
/// \class      ThresholdMean
/// \author     Joachim Danmayr
/// \brief      Also called intermeans
///             Iterative procedure based on the isodata algorithm [T.W. Ridler, S. Calvard, Picture
///             thresholding using an iterative selection method, IEEE Trans. System, Man and
///             Cybernetics, SMC-8 (1978) 630-632.]
///             The procedure divides the image into objects and background by taking an initial threshold,
///             then the averages of the pixels at or below the threshold and pixels above are computed.
///             The averages of those two values are computed, the threshold is incremented and the
///             process is repeated until the threshold is larger than the composite average. That is,
///              threshold = (average background + average objects)/2
///             The code in ImageJ that implements this function is the getAutoThreshold() method in the ImageProcessor class.
///
///             From: Tim Morris (dtm@ap.co.umist.ac.uk)
///             Subject: Re: Thresholding method?
///             posted to sci.image.processing on 1996/06/24
///             The algorithm implemented in NIH Image sets the threshold as that grey
///             value, G, for which the average of the averages of the grey values
///             below and above G is equal to G. It does this by initialising G to the
///             lowest sensible value and iterating:
///
///              L = the average grey value of pixels with intensities < G
///              H = the average grey value of pixels with intensities > G
///              is G = (L + H)/2?
///              yes => exit
///              no => increment G and repeat
///
/// \ref    Ported from https://imagej.net/ij/developer/source/ij/process/AutoThresholder.java.html
///
class ThresholdIsoData final
{
public:
  static uint16_t calcThresholdValue(const cv::Mat &histogram)
  {
    int i;
    int l;
    int totl;
    int g = 0;
    double toth;
    double h;
    for(i = 1; i < 256; i++) {
      if(histogram.at<float>(i) > 0) {
        g = i + 1;
        break;
      }
    }
    while(true) {
      l    = 0;
      totl = 0;
      for(i = 0; i < g; i++) {
        totl = totl + histogram.at<float>(i);
        l    = l + (histogram.at<float>(i) * i);
      }
      h    = 0;
      toth = 0;
      for(i = g + 1; i < 256; i++) {
        toth += histogram.at<float>(i);
        h += (static_cast<double>(histogram.at<float>(i)) * i);
      }
      if(totl > 0 && toth > 0) {
        l /= totl;
        h /= toth;
        if(g == static_cast<int>(std::round((l + h) / 2.0))) {
          break;
        }
      }
      g++;
      if(g > 254) {
        return -1;
      }
    }
    return g;
  }
};

}    // namespace joda::cmd
