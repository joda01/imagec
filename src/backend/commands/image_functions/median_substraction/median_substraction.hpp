///
/// \file      median_substraction.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/commands/factory.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "median_substraction_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
/// \ref https://github.com/imagej/ImageJ/blob/master/ij/process/ShortBlitter.java#L14
///
class MedianSubtraction : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit MedianSubtraction(const joda::settings::MedianSubtractSettings &settings) : mSettings(settings)
  {
  }
  virtual ~MedianSubtraction() = default;

  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    auto medianBlurredImageOut = image.clone();
    RankFilter rank;
    rank.rank(medianBlurredImageOut, mSettings.kernelSize, RankFilter::MEDIAN);
    image = image - medianBlurredImageOut;
  }

private:
  /////////////////////////////////////////////////////
  const joda::settings::MedianSubtractSettings &mSettings;
};

}    // namespace joda::cmd
