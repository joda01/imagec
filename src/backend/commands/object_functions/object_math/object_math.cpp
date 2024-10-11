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

#include "object_math.hpp"
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
#include "object_math_settings.hpp"

namespace joda::cmd {

ObjectMath::ObjectMath(const settings::ObjectMathSettings &settings) : mSettings(settings)
{
}

void ObjectMath::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*resultIn*/)
{
  auto &operand01 = context.loadObjectsFromCache()->at(context.getClusterId(mSettings.inputObjectFirst.clusterId));
  image           = cv::Mat::zeros(image.size(), CV_16UC1);
  operand01->createBinaryImage(image, {context.getClassId(mSettings.inputObjectFirst.classId)});

  cv::Mat img2;
  if(mSettings.function != settings::ObjectMathSettings::Function::NOT) {
    auto &operand02 = context.loadObjectsFromCache()->at(context.getClusterId(mSettings.inputObjectSecond.clusterId));
    img2            = cv::Mat::zeros(image.size(), CV_16UC1);
    operand02->createBinaryImage(img2, {context.getClassId(mSettings.inputObjectSecond.classId)});
  }

  switch(mSettings.function) {
    case settings::ObjectMathSettings::Function::NOT:
      cv::bitwise_not(image, image);
      break;
    case settings::ObjectMathSettings::Function::AND:
      cv::bitwise_and(image, img2, image);
      break;
    case settings::ObjectMathSettings::Function::OR:
      cv::bitwise_or(image, img2, image);
      break;
    case settings::ObjectMathSettings::Function::XOR:
      cv::bitwise_xor(image, img2, image);
      break;
  }

  context.setBinaryImage(0, UINT16_MAX);
}

}    // namespace joda::cmd
