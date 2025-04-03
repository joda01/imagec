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
#include "backend/commands/image_functions/blur/blur_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Blur (2D convolution)
///
class Blur : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit Blur(const settings::BlurSettings &settings) : mSettings(settings)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    int kernel[3]{0};    //= {-1, -1, -1, -1, 12, -1, -1, -1, -1};
    for(int n = 0; n < mSettings.repeat; n++) {
      if(mSettings.mode == settings::BlurSettings::Mode::GAUSSIAN) {
        cv::GaussianBlur(image, image, cv::Size(mSettings.kernelSize, mSettings.kernelSize), 0);
      } else if(mSettings.kernelSize == 3) {
        filter3x3(image, mSettings.mode, kernel, 3);
      } else {
        cv::blur(image, image, cv::Size(mSettings.kernelSize, mSettings.kernelSize));
      }
    }
  }

private:
  /////////////////////////////////////////////////////
  void filter3x3(cv::Mat &imageIn, joda::settings::BlurSettings::Mode type, int *kernel, int kernelArraySize) const;

  /////////////////////////////////////////////////////
  const settings::BlurSettings &mSettings;
};

}    // namespace joda::cmd
