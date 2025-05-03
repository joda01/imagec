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

  for(const auto &classIdIn : mSettings.inputClasses) {
    if(!store.contains(context.getClassId(classIdIn))) {
      continue;
    }
    for(const auto &classIdInSecond : mSettings.inputClassesSecond) {
      if(!store.contains(context.getClassId(classIdInSecond))) {
        continue;
      }

      auto *classsObjects       = store.at(context.getClassId(classIdIn)).get();
      auto *classsObjectsSecond = store.at(context.getClassId(classIdInSecond)).get();

      // Iterate over each object and calc the distance to each other
      for(auto &object : *classsObjects) {
        for(auto &objectSecond : *classsObjectsSecond) {
          object.measureDistanceAndAdd(objectSecond);
        }
      }
    }
  }
}

}    // namespace joda::cmd
