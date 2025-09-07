///
/// \file      threshold.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <opencv2/core/hal/interface.h>
#include <algorithm>
#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "threshold_adaptive_settings.hpp"

namespace joda::cmd {

///
/// \class      ThresholdAdaptive
/// \author     Joachim Danmayr
/// \brief      Base class for thershold calculation
///
class ThresholdAdaptive : public Command
{
public:
  explicit ThresholdAdaptive(const settings::ThresholdAdaptiveSettings &settings) : mSettings(settings)
  {
  }
  virtual ~ThresholdAdaptive() = default;

  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*result*/) override
  {
    cv::Mat outputImage = cv::Mat::zeros(image.size(), CV_16UC1);

    for(const auto &threshold : mSettings.modelClasses) {
      cv::Mat thresholdImg(image.size(), CV_16UC1);
      autoThresholdAdaptive(threshold, image, thresholdImg);
      thresholdImg.setTo(threshold.pixelClassId, thresholdImg > 0);
      context.setBinaryImage(0, 0);
      outputImage = cv::max(outputImage, thresholdImg);
    }

    image = std::move(outputImage);
  }

private:
  /////////////////////////////////////////////////////
  void autoThresholdAdaptive(const settings::ThresholdAdaptiveSettings::ThresholdAdaptive &settings, const cv::Mat &image, cv::Mat &outImage);

  /////////////////////////////////////////////////////
  const settings::ThresholdAdaptiveSettings &mSettings;
};

}    // namespace joda::cmd
