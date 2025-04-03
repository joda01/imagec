///
/// \file      rank_filter.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter_algo.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "rank_filter_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Blur (2D convolution)
///
class RankFilter : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit RankFilter(const settings::RankFilterSettings &settings) : mSettings(settings)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    algo::RankFilter rank;
    rank.rank(image, mSettings.radius, static_cast<int>(mSettings.mode));
  }

private:
  /////////////////////////////////////////////////////
  const settings::RankFilterSettings &mSettings;
};

}    // namespace joda::cmd
