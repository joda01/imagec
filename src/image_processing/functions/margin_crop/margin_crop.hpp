///
/// \file      margin_crop.hpp
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

#include "image_processing/functions/function.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::func::img {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class MarginCrop : public Function
{
public:
  /////////////////////////////////////////////////////
  explicit MarginCrop(int marginSize) : mMarginSize(marginSize)
  {
  }
  void execute(cv::Mat &image) const override
  {
    // Calculate the new dimensions for the cropped image
    int newWidth  = image.cols - mMarginSize * 2;    // x pixels from each side
    int newHeight = image.rows - mMarginSize * 2;    // x pixels from each side

    // Create a rectangle to define the region of interest (ROI)
    cv::Rect roi(mMarginSize, mMarginSize, newWidth, newHeight);

    // Crop the image using the ROI
    image = image(roi);
  }

private:
  /////////////////////////////////////////////////////
  int mMarginSize;
};

}    // namespace joda::func::img
