///
/// \file      pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2023-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Abstract base class for a pipeline run
///

#pragma once

#include "image/image.hpp"
#include "image_processing/functions/artificial_intelligence/ai_types.hpp"
#include "reporting/reporting.h"
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

namespace joda::algo {

///
/// \class      Pipeline
/// \author     Joachim Danmayr
/// \brief      Pipeline base class
///
class Detection
{
public:
  struct DetectionResponse
  {
    joda::func::ai::DetectionResults result;
    cv::Mat controlImage;
  };

  /////////////////////////////////////////////////////
  Detection();
  virtual auto execute(const cv::Mat &img) -> DetectionResponse = 0;
};
}    // namespace joda::algo
