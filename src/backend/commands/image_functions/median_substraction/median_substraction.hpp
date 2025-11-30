///
/// \file      median_substraction.hpp
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
#include "backend/commands/factory.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter_algo.hpp"
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
class MedianSubtract : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  explicit MedianSubtract(const joda::settings::MedianSubtractSettings &settings) : mSettings(settings)
  {
  }
  virtual ~MedianSubtract() = default;

  void execute(cv::Mat &image) override
  {
    auto medianBlurredImageOut = image.clone();
    algo::RankFilter rank;
    rank.rank(medianBlurredImageOut, mSettings.kernelSize, algo::RankFilter::MEDIAN);
    image = image - medianBlurredImageOut;
  }

private:
  /////////////////////////////////////////////////////
  const joda::settings::MedianSubtractSettings &mSettings;
};

}    // namespace joda::cmd
