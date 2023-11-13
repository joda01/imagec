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
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>

namespace joda::func::img {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class EdgeDetection : public Function
{
public:
  enum Algorithm
  {
    NONE,
    SOBEL,
    CANNY
  };

  enum Direction
  {
    XY,
    X,
    Y
  };

  /////////////////////////////////////////////////////
  explicit EdgeDetection(Algorithm value, Direction dir) : mValue(value), mDirection(dir)
  {
  }
  void execute(cv::Mat &image) const override
  {
    switch(mValue) {
      case Algorithm::CANNY:
        cv::Canny(image, image, 100, 200, 3, false);
        break;
      case Algorithm::SOBEL:
        cv::Sobel(image, image, CV_64F, 1, 1, 5);
        break;

      default:
        break;
    }
  }

private:
  /////////////////////////////////////////////////////
  Algorithm mValue;
  Direction mDirection;
};

}    // namespace joda::func::img
