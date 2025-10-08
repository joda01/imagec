///
/// \file      image_saver.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <opencv2/core/hal/interface.h>
#include <filesystem>
#include <string>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/image_from_class/image_from_class_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Extracts a binary image from a classifier result.
///
class ImageFromClass : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit ImageFromClass(const settings::ImageFromClassSettings &settings) : mSettings(settings)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    image = 0;
    for(const auto &cl : mSettings.classesIn) {
      cv::Mat tmp = cv::Mat::zeros(image.size(), CV_16UC1);
      result.at(context.getClassId(cl))->createBinaryImage(tmp, 1, joda::atom::ROI::Category::ANY);
      cv::bitwise_or(image, tmp, image);
    }
  }

private:
  const settings::ImageFromClassSettings &mSettings;
};

}    // namespace joda::cmd
