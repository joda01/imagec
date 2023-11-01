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
#include "backend/pipelines/pipeline_detection/pipeline_detection.hpp"

namespace joda::pipeline::detection {

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
  auto execute(const cv::Mat &img, const cv::Mat &imgOriginal,
               const joda::settings::json::ChannelSettings &channelSetting) -> func::DetectionResponse override;
};

}    // namespace joda::pipeline::detection
