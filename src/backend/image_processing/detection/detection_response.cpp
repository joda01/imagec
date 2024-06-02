///
/// \file      detection_response.cpp
/// \author    Joachim Danmayr
/// \date      2024-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Functions on detection response
///

#include "detection_response.hpp"
#include <opencv2/core/types.hpp>

namespace joda::image::detect {

void DetectionResults::createBinaryImage(cv::Mat &img) const
{
  for(const auto &roi : *this) {
    if(roi.isValid()) {
      int left   = roi.getBoundingBox().x;
      int top    = roi.getBoundingBox().y;
      int width  = roi.getBoundingBox().width;
      int height = roi.getBoundingBox().height;

      if(!roi.getMask().empty() && !roi.getBoundingBox().empty() && roi.getBoundingBox().x >= 0 &&
         roi.getBoundingBox().y >= 0 && roi.getBoundingBox().width >= 0 && roi.getBoundingBox().height >= 0 &&
         roi.getBoundingBox().x + roi.getBoundingBox().width <= img.cols &&
         roi.getBoundingBox().y + roi.getBoundingBox().height <= img.rows) {
        try {
          img(roi.getBoundingBox()).setTo(cv::Scalar(255), roi.getMask());
        } catch(const std::exception &ex) {
          std::cout << "PA: " << ex.what() << std::endl;
        }
      }
    }
  }
}

}    // namespace joda::image::detect
