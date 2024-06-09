///
/// \file      margin_crop.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "../../functions/function.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/image_processing/functions/rank_filter/rank_filter.hpp"
#include "backend/settings/preprocessing/functions/median_subtract.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::image::func {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
/// \ref https://github.com/imagej/ImageJ/blob/master/ij/process/ShortBlitter.java#L14
///
class MedianSubtraction : public Function
{
public:
  /////////////////////////////////////////////////////
  explicit MedianSubtraction(const joda::settings::MedianSubtraction &settings) : mSettings(settings)
  {
  }
  virtual ~MedianSubtraction() = default;

  void execute(cv::Mat &image) const override
  {
    auto idStart               = DurationCount::start("MedianSubtraction");
    auto medianBlurredImageOut = image.clone();
    joda::image::func::RankFilter rank;
    rank.rank(medianBlurredImageOut, mSettings.kernelSize, joda::image::func::RankFilter::MEDIAN);
    image = image - medianBlurredImageOut;
    DurationCount::stop(idStart);
  }

private:
  /////////////////////////////////////////////////////
  const joda::settings::MedianSubtraction &mSettings;
};

}    // namespace joda::image::func
