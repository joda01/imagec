///
/// \file      calc_intensity.cpp
/// \author    Joachim Danmayr
/// \date      2024-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#include "calc_count.hpp"
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/image_processing/detection/detection_response.hpp"

namespace joda::pipeline {

///
/// \brief      Calculate the intensity in other channels
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto CalcCount::execute(
    const settings::AnalyzeSettings &,
    const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> &detectionResultsIn) const
    -> joda::image::detect::DetectionResponse
{
  auto id = DurationCount::start("CrossChannelCount");

  if(detectionResultsIn.contains(mReferenceChannelIndex)) {
    auto &myResults =
        const_cast<joda::image::detect::DetectionResponse &>(detectionResultsIn.at(mReferenceChannelIndex));
    for(const auto idxToIntersect : mChannelsToCalcIntensityIn) {
      if(detectionResultsIn.contains(idxToIntersect)) {
        for(image::ROI &roiMe : myResults.result) {
          if(roiMe.isValid()) {
            if(!detectionResultsIn.empty() && !detectionResultsIn.at(idxToIntersect).result.empty()) {
              for(const auto &roiOther : detectionResultsIn.at(idxToIntersect).result) {
                roiMe.calcIntersectionAndAdd(idxToIntersect, &roiOther);
              }
            } else {
              // Just add empty one
              roiMe.calcIntersectionAndAdd(idxToIntersect, nullptr);
            }
          }
        }
      }
    }
  }

  DurationCount::stop(id);
  return {};
}

}    // namespace joda::pipeline
