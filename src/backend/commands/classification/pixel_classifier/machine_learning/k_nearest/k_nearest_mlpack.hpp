///
/// \file      random_forest_mlpack.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "k_nearest.hpp"

namespace joda::ml {

class KNearestMlPack : public KNearest
{
public:
  /////////////////////////////////////////////////////
  using KNearest::KNearest;
  void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses) override;
};

}    // namespace joda::ml
