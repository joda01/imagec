///
/// \file      Intersection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
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

void ObjectTransform::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &resultIn)
{
  auto inputClass  = context.getClassId(mSettings.inputClasses);
  auto outputClass = context.getClassId(mSettings.outputClasses);
  auto &operand01  = context.loadObjectsFromCache()->at(inputClass);

  for(auto &roi : *operand01) {
    switch(mSettings.function) {
      case settings::ObjectTransformSettings::Function::SCALE: {
        if(inputClass == outputClass) {
          roi.resize(mSettings.scaleFactor, mSettings.scaleFactor);
        } else {
          auto newRoi = roi.copy(outputClass, roi.getParentObjectId());
          newRoi.resize(mSettings.scaleFactor, mSettings.scaleFactor);
          resultIn.push_back(newRoi);
        }
      } break;
      case settings::ObjectTransformSettings::Function::DRAW_CIRCLE: {
        if(inputClass == outputClass) {
          roi.drawCircle(mSettings.scaleFactor);
        } else {
          auto newRoi = roi.copy(outputClass, roi.getParentObjectId());
          newRoi.drawCircle(mSettings.scaleFactor);
          resultIn.push_back(newRoi);
        }
      } break;
    }
  }
}

}    // namespace joda::cmd
