///
/// \file      edge_detection.hpp
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
#include "backend/settings/preprocessing/image_functions/edge_detection.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>

namespace joda::image::func {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class EdgeDetection : public Function
{
public:
  /////////////////////////////////////////////////////
  explicit EdgeDetection(const joda::settings::EdgeDetection &settings) : mSetting(settings)
  {
  }
  void execute(cv::Mat &image) const override
  {
    auto id = DurationCount::start("EdgeDetection");
    switch(mSetting.mode) {
      case joda::settings::EdgeDetection::Mode::CANNY:
        cv::Canny(image, image, 100, 200, 3, false);
        break;
      case joda::settings::EdgeDetection::Mode::SOBEL:
        // cv::Sobel(image, image, CV_16UC1, 1, 1, K_SIZE);
        filter3x3(image);
        break;

      default:
        break;
    }
    DurationCount::stop(id);
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int K_SIZE = 3;
  void filter3x3(cv::Mat &image) const;

  /////////////////////////////////////////////////////
  const joda::settings::EdgeDetection &mSetting;
};

}    // namespace joda::image::func
