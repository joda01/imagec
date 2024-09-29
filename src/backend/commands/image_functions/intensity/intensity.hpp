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
class IntensityTransformation : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit IntensityTransformation(const settings::IntensityTransformationSettings &settings) : mSettings(settings)
  {
  }

  void execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*result*/) override;

private:
  /////////////////////////////////////////////////////
  static cv::Mat equalizeHist16Bit(const cv::Mat &src);

  /////////////////////////////////////////////////////
  const settings::IntensityTransformationSettings &mSettings;
};

}    // namespace joda::cmd
