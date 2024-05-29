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

#include "backend/image_processing/roi/roi.hpp"

namespace joda::image::detect {

using DetectionResults = std::vector<ROI>;

enum class ResponseDataValidityEnum
{
  UNKNOWN                  = 1,
  INVALID                  = 2,
  MANUAL_OUT_SORTED        = 3,
  POSSIBLE_NOISE           = 4,
  POSSIBLE_WRONG_THRESHOLD = 5
};

using ResponseDataValidity = std::bitset<32>;

struct DetectionResponse
{
  DetectionResults result;
  cv::Mat originalImage                 = {};
  cv::Mat controlImage                  = {};
  ResponseDataValidity responseValidity = {};
  bool invalidateWholeImage             = false;
};

}    // namespace joda::image::detect
