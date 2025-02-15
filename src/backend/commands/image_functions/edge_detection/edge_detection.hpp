///
/// \file      edge_detection.hpp
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
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include "edge_detection_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class EdgeDetection : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit EdgeDetection(const joda::settings::EdgeDetectionSettings &settings) : mSetting(settings)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    switch(mSetting.mode) {
      case joda::settings::EdgeDetectionSettings::Mode::CANNY:
        canny(image);
        break;
      case joda::settings::EdgeDetectionSettings::Mode::SOBEL:
        // cv::Sobel(image, image, CV_16UC1, 1, 1, K_SIZE);
        filter3x3(image);
        break;

      default:
        break;
    }
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int K_SIZE = 3;
  void filter3x3(cv::Mat &image) const;
  void canny(cv::Mat &image) const;

  /////////////////////////////////////////////////////
  const joda::settings::EdgeDetectionSettings &mSetting;
};

}    // namespace joda::cmd
