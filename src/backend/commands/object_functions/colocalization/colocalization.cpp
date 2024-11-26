///
/// \file      Colocalization.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "colocalization.hpp"
#include <cstddef>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"

#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::cmd {

Colocalization::Colocalization(const settings::ColocalizationSettings &settings) : mSettings(settings)
{
}

void Colocalization::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &resultIn)
{
  const auto &classesToIntersect = mSettings.inputClasses;
  size_t intersectCount          = classesToIntersect.size();
  try {
    int idx = 0;
    auto it = classesToIntersect.begin();
    if(it == classesToIntersect.end()) {
      return;
    }

    if(intersectCount == 1) {
      joda::log::logWarning("At least two channels must be given to calc Colocalization!");
      return;
    }
    atom::SpheralIndex *result = resultIn[context.getClassId(mSettings.outputClass)].get();

    const auto *firstDataBuffer    = context.loadObjectsFromCache()->at(context.getClassId(*it)).get();
    const auto *working            = firstDataBuffer;
    atom::SpheralIndex *resultTemp = nullptr;
    // Directly write to the output buffer
    atom::SpheralIndex buffer01;
    atom::SpheralIndex buffer02;
    if(intersectCount == 2) {
      resultTemp = result;
    } else {
      resultTemp = &buffer01;
    }

    std::optional<std::set<joda::enums::ClassId>> objectClassesMe = std::set<joda::enums::ClassId>{context.getClassId(*it)};

    ++it;
    ++idx;

    for(; it != classesToIntersect.end(); ++it) {
      const auto *objects02 = context.loadObjectsFromCache()->at(context.getClassId(*it)).get();
      working->calcColocalization(context.getActIterator(), objects02, resultTemp, objectClassesMe, {context.getClassId(*it)},
                                  context.getClassId(mSettings.outputClass), mSettings.minIntersection, context.getActTile(), context.getTileSize());
      // In the second run, we have to ignore the object class filter of me, because this are still the filtered objects
      objectClassesMe.reset();
      idx++;
      if(idx >= intersectCount) {
        break;
      }
      const auto *tmpWorking = working;
      working                = resultTemp;
      if(idx + 1 >= intersectCount) {
        resultTemp = result;
      } else {
        if(tmpWorking == firstDataBuffer) {
          // In the first run the working pointer was the loaded data we must change to buffer
          resultTemp = &buffer02;
        } else {
          // Swap the buffer. We know what  we do.
          resultTemp = const_cast<atom::SpheralIndex *>(tmpWorking);
        }
      }
      resultTemp->clear();
    }

  } catch(const std::exception &ex) {
    joda::log::logWarning("Object with ID >< does not exist! What: " + std::string(ex.what()));
  }
}

}    // namespace joda::cmd
