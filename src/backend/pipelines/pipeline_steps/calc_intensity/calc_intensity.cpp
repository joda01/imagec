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
#include "backend/logger/console_logger.hpp"

namespace joda::pipeline {

///
/// \brief      Calculate the intensity in other channels
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto CalcIntensity::execute(const settings::AnalyzeSettings &,
                            const std::map<int, joda::func::DetectionResponse> &detectionResultsIn,
                            const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  if(detectionResultsIn.contains(mReferenceChannelIndex)) {
    auto &myResults = const_cast<joda::func::DetectionResponse &>(detectionResultsIn.at(mReferenceChannelIndex));
    for(const auto idxToIntersect : mChannelsToCalcIntensityIn) {
      if(detectionResultsIn.contains(idxToIntersect)) {
        for(func::ROI &roi : myResults.result) {
          roi.measureAndAddIntensity(idxToIntersect, detectionResultsIn.at(idxToIntersect).originalImage);
        }
      }
    }
  }
  return {};
}

}    // namespace joda::pipeline
