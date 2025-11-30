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
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "intensity_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Blur (2D convolution)
///
class IntensityTransformation : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  explicit IntensityTransformation(const settings::IntensityTransformationSettings &settings) : mSettings(settings)
  {
  }

  void execute(cv::Mat &image) override;

private:
  /////////////////////////////////////////////////////
  static cv::Mat equalizeHist16Bit(const cv::Mat &src);

  /////////////////////////////////////////////////////
  const settings::IntensityTransformationSettings &mSettings;
};

}    // namespace joda::cmd
