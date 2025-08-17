///
/// \file      edge_detection_sobel.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include "backend/commands/command.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include "edge_detection_sobel_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class EdgeDetectionSobel : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit EdgeDetectionSobel(const joda::settings::EdgeDetectionSobelSettings &settings) : mSetting(settings)
  {
  }
  void execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*result*/) override
  {
    if(mSetting.kernelSize > -1) {
      sobel(image);
    } else {
      filter3x3(image);
    }
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int K_SIZE = 3;
  void filter3x3(cv::Mat &image) const;
  void sobel(cv::Mat &image) const;

  /////////////////////////////////////////////////////
  const joda::settings::EdgeDetectionSobelSettings &mSetting;
};

}    // namespace joda::cmd
