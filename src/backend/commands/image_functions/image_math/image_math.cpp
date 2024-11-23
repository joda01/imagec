///
/// \file      Intersection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "image_math.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include "image_math_settings.hpp"

namespace joda::cmd {

ImageMath::ImageMath(const settings::ImageMathSettings &settings) : mSettings(settings)
{
}

void ImageMath::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*resultIn*/)
{
  auto const *img2 = context.loadImageFromCache(mSettings.inputImageSecond);

  switch(mSettings.function) {
    case settings::ImageMathSettings::Function::INVERT:
      cv::bitwise_not(image, image);
      break;
    case settings::ImageMathSettings::Function::AND:
      cv::bitwise_and(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::OR:
      cv::bitwise_or(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::XOR:
      cv::bitwise_xor(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::ADD:
      cv::add(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::SUBTRACT:
      cv::subtract(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::MULTIPLY:
      cv::multiply(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::DIVIDE:
      cv::divide(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::MIN:
      cv::min(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::MAX:
      cv::max(image, img2->image, image);
      break;
    case settings::ImageMathSettings::Function::AVERAGE:
      image = (image + img2->image) / 2.0;
      break;
    case settings::ImageMathSettings::Function::DIFFERENCE_TYPE:
      image = cv::abs(image - img2->image);
      break;
    default:
      break;
  }
}

}    // namespace joda::cmd
