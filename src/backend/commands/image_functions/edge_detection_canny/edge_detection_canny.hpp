///
/// \file      edge_detection.hpp
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
#include "edge_detection_canny_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class EdgeDetectionCanny : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit EdgeDetectionCanny(const joda::settings::EdgeDetectionCannySettings &settings) : mSetting(settings)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    canny(image);
  }

private:
  /////////////////////////////////////////////////////
  void canny(cv::Mat &image) const;

  /////////////////////////////////////////////////////
  const joda::settings::EdgeDetectionCannySettings &mSetting;
};

}    // namespace joda::cmd
