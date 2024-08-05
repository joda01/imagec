

///
/// \file      resize.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "../../functions/function.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::image::func {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class Resizer
{
public:
  /////////////////////////////////////////////////////
  static cv::Mat resizeWithAspectRatio(const cv::Mat &src, int targetWidth, int targetHeight)
  {
    int originalWidth  = src.cols;
    int originalHeight = src.rows;
    if(originalWidth <= targetWidth && originalHeight <= targetHeight) {
      return src;
    }
    // Calculate the aspect ratio
    float aspectRatio = static_cast<float>(originalWidth) / static_cast<float>(originalHeight);

    // Determine new dimensions
    int newWidth, newHeight;

    // Check if width or height is the limiting factor
    if(targetWidth / aspectRatio <= targetHeight) {
      // Width is the limiting factor
      newWidth  = targetWidth;
      newHeight = static_cast<int>(targetWidth / aspectRatio);
    } else {
      // Height is the limiting factor
      newHeight = targetHeight;
      newWidth  = static_cast<int>(targetHeight * aspectRatio);
    }

    // Resize image
    cv::Mat resized;
    cv::resize(src, resized, cv::Size(newWidth, newHeight));
    return resized;
  }

private:
  /////////////////////////////////////////////////////
  int mMarginSize;
};

}    // namespace joda::image::func
