///
/// \file      image_saver.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <bits/iterator_concepts.h>
#include <filesystem>
#include <string>
#include "backend/commands/command.hpp"
#include "backend/commands/functions/image_from_class/image_from_class_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd::functions {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Extracts a binary image from a classifier result.
///
class ImageFromClass : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit ImageFromClass(const ImageFromClassSettings &settings) : mSettings(settings)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, ObjectsListMap &result) override
  {
    auto id = DurationCount::start("ImageFromClass");
    image   = 0;
    result.createBinaryImage(image, mSettings.objectClasses);

    DurationCount::stop(id);
  }

private:
  const ImageFromClassSettings &mSettings;
};

}    // namespace joda::cmd::functions
