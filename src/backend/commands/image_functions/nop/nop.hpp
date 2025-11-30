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
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "nop_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief
///
class Nop : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  explicit Nop(const settings::NopSettings &settings) : mSettings(settings)
  {
  }
  void execute(cv::Mat & /*image*/) override
  {
    for(int32_t n = 0; n < mSettings.repeat; n++) {
      ;
    }
  }

private:
  /////////////////////////////////////////////////////
  const settings::NopSettings &mSettings;
};

}    // namespace joda::cmd
