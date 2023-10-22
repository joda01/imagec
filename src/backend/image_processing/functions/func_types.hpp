///
/// \file      ai_types.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "roi/roi.hpp"

namespace joda::func {

using DetectionResults = std::vector<ROI>;

struct DetectionResponse
{
  joda::func::DetectionResults result;
  cv::Mat originalImage = {};
  cv::Mat controlImage  = {};
};

}    // namespace joda::func
