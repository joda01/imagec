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
/// \brief      Base class for an image processing function
/// \ref https://github.com/imagej/ImageJ/blob/master/ij/process/ShortBlitter.java#L14
///
class MedianSubtraction : public Function
{
public:
  /////////////////////////////////////////////////////
  explicit MedianSubtraction(int kernelSize) : mKernelSize(kernelSize)
  {
  }
  void execute(cv::Mat &image) const override
  {
    cv::Mat medianBlurredImage = image.clone();
    cv::medianBlur(image, medianBlurredImage, mKernelSize /*ksize*/);
    image = image - medianBlurredImage;
  }

private:
  /////////////////////////////////////////////////////
  int mKernelSize;
};

}    // namespace joda::func::img
