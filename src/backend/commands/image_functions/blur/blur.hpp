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

#include <bits/iterator_concepts.h>
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/blur/blur_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Blur (2D convolution)
///
class Blur : public Command
{
public:
  enum Type
  {
    CONVOLVE,
    BLUR_MORE,
    FIND_EDGES
  };

  /////////////////////////////////////////////////////
  explicit Blur(const settings::BlurSettings &settings) : mRepeat(settings.repeat), mKernelSize(settings.kernelSize)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    auto id = DurationCount::start("Blur");
    int kernel[3]{0};    //= {-1, -1, -1, -1, 12, -1, -1, -1, -1};
    for(int n = 0; n < mRepeat; n++) {
      if(mKernelSize == 3) {
        filter3x3(image, BLUR_MORE, kernel, 3);
      } else {
        cv::blur(image, image, cv::Size(mKernelSize, mKernelSize));
      }
    }
    DurationCount::stop(id);
  }

private:
  /////////////////////////////////////////////////////
  void filter3x3(cv::Mat &imageIn, int type, int *kernel, int kernelArraySize) const;
  /////////////////////////////////////////////////////
  int mRepeat;
  int mKernelSize;
};

}    // namespace joda::cmd
