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
    auto *objectList                                  = context.loadObjectsFromCache();
    std::unique_ptr<atom::SpheralIndex> &objectsInOut = objectList->at(context.getClassId(inputClassification));

    if(mSettings.intersection.inputClassesIntersectWith.empty()) {
      std::vector<atom::ROI> roisToEnter;
      std::vector<atom::ROI *> roisToRemove;

      for(auto &roi : *objectsInOut) {
        if(settings::ClassifierFilter::doesFilterMatch(context, roi, mSettings.metrics, mSettings.intensity)) {
          if(mSettings.mode == settings::ReclassifySettings::Mode::RECLASSIFY_MOVE) {
            // We have to reenter to organize correct in the map of objects
            auto newRoi = roi.clone(context.getClassId(mSettings.newClassId), roi.getParentObjectId());
            roisToEnter.emplace_back(std::move(newRoi));
            roisToRemove.emplace_back(&roi);
          } else if(mSettings.mode == settings::ReclassifySettings::Mode::RECLASSIFY_COPY) {
            auto newRoi = roi.copy(context.getClassId(mSettings.newClassId), roi.getParentObjectId());
            roisToEnter.emplace_back(std::move(newRoi));    // Store the ROIs we want to enter
          }
        }
      }
      // Enter the rois from the temp storage
      for(const auto &roi : roisToEnter) {
        objectList->push_back(roi);
      }

      // Remove ROIs
      for(const auto *roi : roisToRemove) {
        objectsInOut->erase(roi);
      }

    } else {
      for(const auto &intersectWithClasssId : mSettings.intersection.inputClassesIntersectWith) {
        auto *intersectWith = context.loadObjectsFromCache()->at(context.getClassId(intersectWithClasssId)).get();
        objectsInOut->calcIntersection(objectList, context, mSettings.mode, mSettings.hierarchyMode, intersectWith,
                                       {context.getClassId(inputClassification)}, {context.getClassId(intersectWithClasssId)},
                                       mSettings.intersection.minIntersection, mSettings.metrics, mSettings.intensity,
                                       context.getClassId(mSettings.newClassId));
      }
    }
  }
}

}    // namespace joda::cmd
