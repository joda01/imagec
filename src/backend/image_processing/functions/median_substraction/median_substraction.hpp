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
  virtual ~MedianSubtraction() = default;

  void execute(cv::Mat &image) const override
  {
    // when ksize is 3 or 5, the image depth should be CV_8U, CV_16U, or CV_32F, for larger aperture sizes, it can only
    // be CV_8U.
    bool reduce = mKernelSize > 5;
    cv::Mat medianBlurredImage(image.size(), reduce ? CV_8UC1 : CV_16UC1);
    double minVal = 0, maxVal = 0;

    if(reduce) {
      // image.convertTo(medianBlurredImage, CV_8UC1, static_cast<float>(UCHAR_MAX) / static_cast<float>(UINT16_MAX));

      cv::minMaxLoc(image, &minVal, &maxVal);    // Find minimum and maximum pixel values

      image.convertTo(medianBlurredImage, CV_8UC1, 255.0 / (maxVal - minVal),
                      -minVal * 255.0 / (maxVal - minVal));    // Scale and shift pixel values

    } else {
      medianBlurredImage = image.clone();
    }
    cv::medianBlur(medianBlurredImage, medianBlurredImage, mKernelSize);
    if(reduce) {
      cv::Mat medianBlurredImageOut(image.size(), CV_16UC1);
      medianBlurredImage.convertTo(medianBlurredImageOut, CV_16UC1, (maxVal - minVal) / 255.0, minVal);
      image = image - medianBlurredImageOut;
    } else {
      image = image - medianBlurredImage;
    }
  }

private:
  /////////////////////////////////////////////////////
  int mKernelSize;
};

}    // namespace joda::func::img
