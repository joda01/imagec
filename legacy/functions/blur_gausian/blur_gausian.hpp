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

///

#pragma once

#include "../../image_functions/function.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/settings/preprocessing/image_functions/gaussian_blur.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::image::func {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Blur (2D convolution)
///
class GaussianBlur : public Function
{
public:
  enum Type
  {
    CONVOLVE,
    BLUR_MORE,
    FIND_EDGES
  };

  /////////////////////////////////////////////////////
  explicit GaussianBlur(const joda::settings::GaussianBlur &settings) : mSettings(settings)
  {
  }
  void execute(cv::Mat &image) const override
  {
    auto id = DurationCount::start("GaussianBlur");
    for(int n = 0; n < mSettings.repeat; n++) {
      cv::GaussianBlur(image, image, cv::Size(mSettings.kernelSize, mSettings.kernelSize), 0);
    }
    DurationCount::stop(id);
  }

private:
  /////////////////////////////////////////////////////
  const joda::settings::GaussianBlur &mSettings;
};

}    // namespace joda::image::func
