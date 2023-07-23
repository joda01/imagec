///
/// \file      nucleus_count.hpp
/// \author    Joachim Danmayr
/// \date      2023-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Pipeline which implements an AI based nuclues counter
///

#include <cmath>
#include <mutex>
#include "image_processing/functions/detection/ai_object_detection/ai_object_detection.h"
#include "image_processing/object_detection/detection.hpp"

namespace joda::algo {

///
/// \class      NucleusCounter
/// \author     Joachim Danmayr
/// \brief      Nucleus counter pipeline
///
class NucleusCounter : public Detection
{
public:
  /////////////////////////////////////////////////////
  using Detection::Detection;
  auto execute(const cv::Mat &img, const joda::settings::json::ChannelDetection &channelSetting)
      -> func::DetectionResponse override;
};

}    // namespace joda::algo
