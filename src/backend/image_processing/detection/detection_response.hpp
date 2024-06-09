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

#include <memory>
#include "backend/image_processing/roi/roi.hpp"
#include "backend/image_processing/roi/spartial_hash.hpp"

namespace joda::image::detect {

class DetectionResults : public SpatialHash
{
public:
  using SpatialHash::SpatialHash;
  void createBinaryImage(cv::Mat &img) const;
};

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
  std::unique_ptr<DetectionResults> result = std::make_unique<DetectionResults>();
  cv::Mat originalImage                    = {};
  cv::Mat controlImage                     = {};
  ResponseDataValidity responseValidity    = {};
  bool invalidateWholeImage                = false;
};

}    // namespace joda::image::detect
