///
/// \file      Colocalization.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "colocalization.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include "backend/artifacts/object_list/object_list.hpp"

#include "backend/commands/object_functions/colocalization/colocalization_settings.hpp"
#include "backend/enums/enums_classes.hpp"
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
  size_t intersectCount          = 0;
  for(const auto &sect : classesToIntersect) {
    if(sect.inputClassId != enums::ClassIdIn::UNDEFINED) {
      intersectCount++;
    }
  }
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

    const auto *firstDataBuffer    = context.loadObjectsFromCache()->at(context.getClassId(it->inputClassId)).get();
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

    std::optional<std::set<joda::enums::ClassId>> objectClassesMe = std::set<joda::enums::ClassId>{context.getClassId(it->inputClassId)};
    ++it;
    ++idx;

    for(; it != classesToIntersect.end(); ++it) {
      if(it->inputClassId == enums::ClassIdIn::UNDEFINED) {
        continue;
      }
      if(!context.loadObjectsFromCache()->contains(context.getClassId(it->inputClassId))) {
        continue;
      }
      const auto *objects02 = context.loadObjectsFromCache()->at(context.getClassId(it->inputClassId)).get();
      working->calcColocalization(context.getActIterator(), objects02, resultTemp, objectClassesMe, {context.getClassId(it->inputClassId)},
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

    std::vector<atom::ROI> roisToEnter;
    std::vector<const atom::ROI *> roisToRemove;
    std::set<uint64_t> intersecting;

    //
    // In the `result` we now have the intersecting ROIs.
    // In the next step we want to extract the origin objects and apply a reclassify copy/move.
    // The origin id of the new object is the coloc.
    //
    {
      auto getNewClassIdForMyClassId = [&, this](enums::ClassId inClass) -> enums::ClassId {
        for(const auto &classs : mSettings.inputClasses) {
          if(context.getClassId(classs.inputClassId) == inClass) {
            return context.getClassId(classs.newClassId);
          }
        }
        return enums::ClassId::NONE;
      };

      //
      // Move or copy coloc classes
      //
      for(auto &colocRois : *result) {
        uint64_t trackingID = atom::ROI::generateNewTrackingId();
        colocRois.setTrackingId(trackingID);
        for(const auto &linked : colocRois.getLinkedRois()) {
          intersecting.emplace(linked->getObjectId());
          auto newClassId = getNewClassIdForMyClassId(linked->getClassId());
          if(newClassId != enums::ClassId::UNDEFINED && newClassId != enums::ClassId::NONE) {
            if(mSettings.mode == settings::ColocalizationSettings::Mode::RECLASSIFY_MOVE) {
              // We have to reenter to organize correct in the map of objects
              auto newRoi = linked->clone(newClassId, linked->getParentObjectId());
              newRoi.setTrackingId(trackingID);
              roisToEnter.emplace_back(std::move(newRoi));
              roisToRemove.emplace_back(linked);
            } else if(mSettings.mode == settings::ColocalizationSettings::Mode::RECLASSIFY_COPY) {
              auto newRoi = linked->copy(newClassId, linked->getParentObjectId());
              /// \todo Should to origin EV also get the tracking ID or not?
              newRoi.setTrackingId(trackingID);
              roisToEnter.emplace_back(std::move(newRoi));    // Store the ROIs we want to enter
            }
          }
        }

        colocRois.clearLinkedWith();
      }
    }
    //
    // Move or copy not not coloc classes
    //
    {
      auto getNewClassIdForMyNotIntersectingClassId = [&, this](enums::ClassId inClass) -> enums::ClassId {
        for(const auto &classs : mSettings.inputClasses) {
          if(context.getClassId(classs.inputClassId) == inClass) {
            return context.getClassId(classs.newClassIdNotIntersecting);
          }
        }
        return enums::ClassId::NONE;
      };

      for(auto const &cl : mSettings.inputClasses) {
        if(!context.loadObjectsFromCache()->contains(context.getClassId(cl.inputClassId))) {
          continue;
        }
        const auto &objects = context.loadObjectsFromCache()->at(context.getClassId(cl.inputClassId));
        for(const auto &roi : *objects) {
          if(!intersecting.contains(roi.getObjectId())) {
            auto newClassId = getNewClassIdForMyNotIntersectingClassId(roi.getClassId());
            if(newClassId != enums::ClassId::UNDEFINED && newClassId != enums::ClassId::NONE) {
              if(mSettings.mode == settings::ColocalizationSettings::Mode::RECLASSIFY_MOVE) {
                auto newRoi = roi.clone(newClassId, roi.getParentObjectId());
                roisToEnter.emplace_back(std::move(newRoi));
                roisToRemove.emplace_back(&roi);
              } else if(mSettings.mode == settings::ColocalizationSettings::Mode::RECLASSIFY_COPY) {
                auto newRoi = roi.copy(newClassId, roi.getParentObjectId());
                roisToEnter.emplace_back(std::move(newRoi));    // Store the ROIs we want to enter
              }
            }
          }
        }
      }
    }

    // Enter the rois from the temp storage
    for(const auto &roi : roisToEnter) {
      resultIn.push_back(roi);
    }

    // Remove ROIs
    for(const auto *roi : roisToRemove) {
      resultIn.erase(roi);
    }

  } catch(const std::exception &ex) {
    joda::log::logWarning("Object with ID >< does not exist! What: " + std::string(ex.what()));
  }
}

}    // namespace joda::cmd
