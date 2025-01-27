///
/// \file      ai_detector.hpp
/// \author    Joachim Danmyr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace joda::ai {

class AiSegmentation
{
public:
  struct Result
  {
    cv::Rect boundingBox;
    cv::Mat mask;
    std::vector<cv::Point> contour;
    int32_t classId;
    float probability;
  };
  virtual auto execute(const cv::Mat &originalImage) -> std::vector<Result> = 0;
};

}    // namespace joda::ai
