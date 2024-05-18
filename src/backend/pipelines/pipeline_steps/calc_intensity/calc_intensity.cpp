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

#include "calc_intensity.hpp"
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::pipeline {

///
/// \brief      Calculate the intensity in other channels
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto CalcIntensity::execute(
    const settings::AnalyzeSettings &,
    const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> &detectionResultsIn) const
    -> joda::image::detect::DetectionResponse
{
  auto id = DurationCount::start("CrossChannelIntensity");

  if(detectionResultsIn.contains(mReferenceChannelIndex)) {
    auto &myResults =
        const_cast<joda::image::detect::DetectionResponse &>(detectionResultsIn.at(mReferenceChannelIndex));
    for(const auto idxToIntersect : mChannelsToCalcIntensityIn) {
      if(detectionResultsIn.contains(idxToIntersect)) {
        for(image::ROI &roi : myResults.result) {
          if(roi.isValid()) {
            roi.measureAndAddIntensity(idxToIntersect, detectionResultsIn.at(idxToIntersect).originalImage);
          } else {
            // This is not a valid area. Do not measure intensity
            // roi.measureAndAddIntensity(idxToIntersect, {});
          }
        }
      }
    }
  }

  DurationCount::stop(id);
  return {};
}

}    // namespace joda::pipeline
