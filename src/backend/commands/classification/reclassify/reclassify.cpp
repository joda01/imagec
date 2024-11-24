///
/// \file      Reclassify.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "reclassify.hpp"
#include <cstddef>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::cmd {

Reclassify::Reclassify(const settings::ReclassifySettings &settings) : mSettings(settings)
{
}

void Reclassify::execute(processor::ProcessContext &context, cv::Mat & /*image*/, atom::ObjectList & /*resultIn*/)
{
  for(const auto &inputClassification : mSettings.inputClasses) {
    auto &objectsInOut = context.loadObjectsFromCache()->at(context.getClassId(inputClassification));

    if(mSettings.intersection.inputClassesIntersectWith.empty()) {
      for(auto &roi : *objectsInOut) {
        if(settings::ClassifierFilter::doesFilterMatch(context, roi, mSettings.metrics, mSettings.intensity)) {
          if(mSettings.intersection.inputClassesIntersectWith.empty()) {
            roi.setClass(context.getClassId(mSettings.newClassId));
          }
        }
      }
    } else {
      for(const auto &intersectWithClasssId : mSettings.intersection.inputClassesIntersectWith) {
        auto *intersectWith = context.loadObjectsFromCache()->at(context.getClassId(intersectWithClasssId)).get();
        objectsInOut->calcIntersection(context, mSettings.mode, intersectWith, {context.getClassId(inputClassification)},
                                       {context.getClassId(intersectWithClasssId)}, mSettings.intersection.minIntersection, mSettings.metrics,
                                       mSettings.intensity, context.getClassId(mSettings.newClassId));
      }
    }
  }
}

}    // namespace joda::cmd
