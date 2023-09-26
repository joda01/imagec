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
  /////////////////////////////////////////////////////
  explicit Blur(int filterKernelSize) : mFilterKernelSize(filterKernelSize)
  {
  }
  void execute(cv::Mat &image) const override
  {
    cv::GaussianBlur(image, image, cv::Size(mFilterKernelSize, mFilterKernelSize), 0);
  }

private:
  /////////////////////////////////////////////////////
  int mFilterKernelSize;
};

}    // namespace joda::func::img
