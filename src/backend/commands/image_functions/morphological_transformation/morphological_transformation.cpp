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

#include "morphological_transformation.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "morphological_transformation_settings.hpp"

namespace joda::cmd {

MorphologicalTransform::MorphologicalTransform(const settings::MorphologicalTransformSettings &settings) : mSettings(settings)
{
}

void MorphologicalTransform::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*resultIn*/)
{
  auto function = cv::MORPH_CLOSE;
  switch(mSettings.function) {
    case settings::MorphologicalTransformSettings::Function::UNKNOWN:
      THROW("Unknow morphological function!");
    case settings::MorphologicalTransformSettings::Function::ERODE:
      function = cv::MORPH_ERODE;
      break;
    case settings::MorphologicalTransformSettings::Function::DILATE:
      function = cv::MORPH_DILATE;
      break;
    case settings::MorphologicalTransformSettings::Function::OPEN:
      function = cv::MORPH_OPEN;
      break;
    case settings::MorphologicalTransformSettings::Function::CLOSE:
      function = cv::MORPH_CLOSE;
      break;
    case settings::MorphologicalTransformSettings::Function::GRADIENT:
      function = cv::MORPH_GRADIENT;
      break;
    case settings::MorphologicalTransformSettings::Function::TOPHAT:
      function = cv::MORPH_TOPHAT;
      break;
    case settings::MorphologicalTransformSettings::Function::BLACKHAT:
      function = cv::MORPH_BLACKHAT;
      break;
    case settings::MorphologicalTransformSettings::Function::HITMISS:
      break;
      function = cv::MORPH_HITMISS;
      break;
  }

  auto mode = cv::MORPH_ELLIPSE;
  switch(mSettings.shape) {
    case settings::MorphologicalTransformSettings::Shape::RECTANGLE:
      mode = cv::MORPH_RECT;
      break;
    case settings::MorphologicalTransformSettings::Shape::CROSS:
      mode = cv::MORPH_CROSS;
      break;
    case settings::MorphologicalTransformSettings::Shape::ELLIPSE:
      mode = cv::MORPH_ELLIPSE;
      break;
  }

  // Create an elliptical structuring element of size 3x3
  cv::Mat kernel = cv::getStructuringElement(mode, cv::Size(mSettings.kernelSize, mSettings.kernelSize));

  // Apply the morphological operation
  cv::morphologyEx(image, image, function, kernel, cv::Point(-1, -1), mSettings.iterations);
}

}    // namespace joda::cmd
