///
/// \file      threshold.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "threshold.hpp"

namespace joda::func::img {

///
/// \class      Threshold
/// \author     Joachim Danmayr
/// \brief      Base class for thershold calculation
///
class ThresholdManual final : public Threshold
{
public:
  /////////////////////////////////////////////////////
  using Threshold::Threshold;

private:
  /////////////////////////////////////////////////////
  [[nodiscard]] uint16_t autoThreshold(const cv::Mat & /*srcImg*/) const override
  {
    return getMinThreshold();
  }

  [[nodiscard]] uint16_t calcThresholdValue(cv::Mat & /*histogram*/) const override
  {
    return getMinThreshold();
  }
};

}    // namespace joda::func::img
