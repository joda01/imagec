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
#include "backend/logger/console_logger.hpp"

namespace joda::pipeline {

///
/// \brief      Calculate the intensity in other channels
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto CalcCount::execute(const settings::json::AnalyzeSettings &,
                        const std::map<int, joda::func::DetectionResponse> &detectionResultsIn,
                        const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  if(detectionResultsIn.contains(mReferenceChannelIndex)) {
    auto &myResults = const_cast<joda::func::DetectionResponse &>(detectionResultsIn.at(mReferenceChannelIndex));
    for(const auto idxToIntersectStr : mChannelsToCalcIntensityIn) {
      int idxToIntersect = -1;
      try {
        idxToIntersect = stoi(idxToIntersectStr);
      } catch(...) {
        if(idxToIntersectStr.size() == 1) {
          idxToIntersect =
              settings::json::PipelineStepSettings::INTERSECTION_INDEX_OFFSET + (idxToIntersectStr.at(0) - 'A');
        } else {
          joda::log::logWarning("This is not a valid intersecting channel!");
        }
      }
      if(detectionResultsIn.contains(idxToIntersect)) {
        std::cout << "Ref Int " + std::to_string(mReferenceChannelIndex) << " | " << std::to_string(idxToIntersect)
                  << " | " << std::to_string(myResults.result.size()) << std::endl;
        for(func::ROI &roiMe : myResults.result) {
          if(!detectionResultsIn.empty() && !detectionResultsIn.at(idxToIntersect).result.empty()) {
            for(const auto &roiOther : detectionResultsIn.at(idxToIntersect).result) {
              roiMe.calcIntersectionAndAdd(idxToIntersect, &roiOther);
            }
          } else {
            // Empty
            roiMe.calcIntersectionAndAdd(idxToIntersect, nullptr);
          }
        }
      }
    }
  }
  return {};
}

}    // namespace joda::pipeline
