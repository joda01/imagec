///
/// \file      nucleus_count.cpp
/// \author    Joachim Danmayr
/// \date      2023-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Pipeline which implements an AI based nucleus counter
///

#include "nucleus_count.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string_view>
#include "duration_count/duration_count.h"

namespace joda::algo {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
auto NucleusCounter::execute(const cv::Mat &img) -> func::DetectionResponse
{
  auto enhancedContrast = img;
  joda::func::ai::ObjectDetector obj("imagec_models/nucleus_detection_ex_vivo_v1.onnx",
                                     {"nuclues", "nucleus_no_focus"});
  return obj.forward(enhancedContrast);
}
}    // namespace joda::algo
