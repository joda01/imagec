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

#include "../../functions/function.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::func::img {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Blur (2D convolution)
///
class Blur : public Function
{
public:
  enum Type
  {
    CONVOLVE,
    BLUR_MORE,
    FIND_EDGES
  };

  /////////////////////////////////////////////////////
  explicit Blur(int repeat) : mRepeat(repeat)
  {
  }
  void execute(cv::Mat &image) const override
  {
    int kernel[3];    //= {-1, -1, -1, -1, 12, -1, -1, -1, -1};
    for(int n = 0; n < mRepeat; n++) {
      // cv::blur(image, image, cv::Size(3, 3));
      filter3x3(image, BLUR_MORE, kernel, 3);
    }
  }

private:
  /////////////////////////////////////////////////////
  void filter3x3(cv::Mat &image, int type, int *kernel, int kernelArraySize) const;
  /////////////////////////////////////////////////////
  int mRepeat;
};

}    // namespace joda::func::img
