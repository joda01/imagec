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

#include <string>
#include <vector>
#include "settings/channel_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

namespace joda::func {

using Boxes      = cv::Rect;
using Confidence = float;
using ClassId    = int;

enum class ParticleValidity : int
{
  UNKNOWN              = 0,
  VALID                = 0x01,
  TOO_SMALL            = 0x02,
  TOO_BIG              = 0x04,
  TOO_LESS_CIRCULARITY = 0x08
};

struct Detection
{
  uint32_t index;           ///< Index in the prediction array
  Confidence confidence;    ///< Probability
  ClassId classId;          ///< Class id
  Boxes box;                ///< Rectangle around the prediction
  cv::Mat boxMask;          ///< Segmentation mask
  float intensity;          ///< Avg intensity of the masking area
  float intensityMin;       ///< Min intensity of the masking area
  float intensityMax;       ///< Max intensity of the masking area
  float areaSize;           ///< size of the masking area [px^2 / px^3]
  float circularity;        ///< Circularity of the masking area [0-1]
  ParticleValidity validity = ParticleValidity::UNKNOWN;

  ///
  /// \brief     Applies particle filter and sets the validity
  ///            based on the detection results
  /// \author    Joachim Danmayr
  ///
  void applyParticleFilter(const joda::settings::json::ChannelFiltering &filter)
  {
    validity = ParticleValidity::UNKNOWN;
    if(areaSize > filter.getMaxParticleSize()) {
      validity =
          static_cast<ParticleValidity>(static_cast<int>(validity) | static_cast<int>(ParticleValidity::TOO_BIG));
    }
    if(areaSize < filter.getMinParticleSize()) {
      validity =
          static_cast<ParticleValidity>(static_cast<int>(validity) | static_cast<int>(ParticleValidity::TOO_SMALL));
    }
    if(circularity < filter.getMinCircularity()) {
      validity = static_cast<ParticleValidity>(static_cast<int>(validity) |
                                               static_cast<int>(ParticleValidity::TOO_LESS_CIRCULARITY));
    }
    if(validity == ParticleValidity::UNKNOWN) {
      validity = ParticleValidity::VALID;
    }

    // filter.getSnapAreaSize();
  }
};

using DetectionResults = std::vector<Detection>;

struct DetectionResponse
{
  joda::func::DetectionResults result;
  cv::Mat controlImage;
};

using ProcessingResult = std::map<uint32_t, DetectionResponse>;

}    // namespace joda::func
