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

enum class ResponseDataValidity : int
{
  UNKNOWN                  = 0,
  VALID                    = 0x01,
  POSSIBLE_NOISE           = 0x02,
  POSSIBLE_WRONG_THRESHOLD = 0x04
};

struct DetectionResponse
{
  DetectionResults result;
  cv::Mat originalImage                 = {};
  cv::Mat controlImage                  = {};
  ResponseDataValidity responseValidity = ResponseDataValidity::VALID;
  bool invalidateWholeImage             = false;
};

}    // namespace joda::image::detect
