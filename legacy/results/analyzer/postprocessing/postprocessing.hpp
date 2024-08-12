///
/// \file      postprocessing.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <string>
#include <opencv2/core/mat.hpp>

namespace joda::image::postprocessing {

class PostProcessor
{
public:
  static cv::Mat markPositionInImage(const cv::Mat &, int x, int y);
};

}    // namespace joda::image::postprocessing
