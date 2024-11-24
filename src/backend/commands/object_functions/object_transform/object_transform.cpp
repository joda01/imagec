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

#include "object_transform.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"

#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include "object_transform_settings.hpp"

namespace joda::cmd {

ObjectTransform::ObjectTransform(const settings::ObjectTransformSettings &settings) : mSettings(settings)
{
}

void ObjectTransform::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*resultIn*/)
{
  auto &operand01 = context.loadObjectsFromCache()->at(context.getClassId(mSettings.inputClasses));

  for(auto &roi : *operand01) {
    switch(mSettings.function) {
      case settings::ObjectTransformSettings::Function::SCALE:
        roi.resize(mSettings.scaleFactor, mSettings.scaleFactor);
        break;
    }
  }
}

}    // namespace joda::cmd
