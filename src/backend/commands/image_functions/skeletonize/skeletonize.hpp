///
/// \file      margin_crop.hpp
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
#include "backend/commands/image_functions/skeletonize/skeletonize_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Skeletonize (2D convolution)
///
class Skeletonize : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  explicit Skeletonize(const settings::SkeletonizeSettings &settings) : mSettings(settings)
  {
  }
  void execute(cv::Mat &image) override;

private:
  /////////////////////////////////////////////////////
  const settings::SkeletonizeSettings &mSettings;
};

}    // namespace joda::cmd
