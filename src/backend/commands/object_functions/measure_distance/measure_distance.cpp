///
/// \file      measure_distance.cpp
/// \author    Joachim Danmayr
/// \date      2025-05-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "measure_distance.hpp"
#include "backend/commands/object_functions/measure_distance/measure_distance_settings.hpp"

namespace joda::cmd {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MeasureDistance::execute(processor::ProcessContext &context, cv::Mat & /*image*/, atom::ObjectList &result)
{
  auto &store = *context.loadObjectsFromCache();

  auto *classsObjectFrom = store.at(context.getClassId(mSettings.inputClassFrom)).get();
  auto *classsObjectsTo  = store.at(context.getClassId(mSettings.inputClassTo)).get();

  // Iterate over each object and calc the distance to each other
  for(auto &objectFrom : *classsObjectFrom) {
    for(auto &objectTo : *classsObjectsTo) {
      if(mSettings.condition == settings::DistanceMeasureConditions::ALL) {
        objectFrom.measureDistanceAndAdd(objectTo);
      } else if(mSettings.condition == settings::DistanceMeasureConditions::INTERSECTING) {
        if(objectFrom.isIntersecting(objectTo, mSettings.minIntersection)) {
          objectFrom.measureDistanceAndAdd(objectTo);
        }
      } else if(mSettings.condition == settings::DistanceMeasureConditions::SAME_PARENT_ID) {
        if(objectFrom.getParentObjectId() == objectTo.getParentObjectId()) {
          objectFrom.measureDistanceAndAdd(objectTo);
        }
      } else if(mSettings.condition == settings::DistanceMeasureConditions::IS_TO_PARENT_OF) {
        if(objectTo.getObjectId() == objectFrom.getParentObjectId()) {
          objectFrom.measureDistanceAndAdd(objectTo);
        }
      }
    }
  }
}

}    // namespace joda::cmd
