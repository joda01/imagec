///
/// \file      calc_intersection.cpp
/// \author    Joachim Danmayr
/// \date      2023-09-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "calc_intersection.hpp"
#include <string>
#include "../../image_processing/functions/detection/detection.hpp"
#include "../../reporting/reporting.h"
#include "backend/image_processing/functions/func_types.hpp"

namespace joda::pipeline {

CalcIntersection::CalcIntersection(const std::set<int32_t> &indexesToIntersect, float minIntersection) :
    mIndexesToIntersect(indexesToIntersect), mMinIntersection(minIntersection)
{
}

auto CalcIntersection::execute(const settings::json::AnalyzeSettings &settings,
                               const std::map<int, joda::func::DetectionResponse> &detectionResultsIn,
                               const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  joda::func::DetectionResponse resp;

  const joda::func::DetectionResponse *ch1;
  const joda::func::DetectionResponse *ch2;
  if(detectionResultsIn.at(0).result.size() > detectionResultsIn.at(13).result.size()) {
    ch1 = &detectionResultsIn.at(0);
    ch2 = &detectionResultsIn.at(13);
  } else {
    ch1 = &detectionResultsIn.at(13);
    ch2 = &detectionResultsIn.at(0);
  }

  //
  // Calculate the intersection
  //
  for(auto const &roi01 : ch1->result) {
    for(auto const &roi02 : ch2->result) {
      if(roi01.isValid() && roi02.isValid()) {
        auto [colocROI, ok] = roi01.calcIntersection(roi02, ch1->originalImage, mMinIntersection);
        // We only log the first occurency of intersestion. Intersection over more particles is not logged yet
        if(ok) {
          resp.result.push_back(colocROI);
          break;
        }
      }
    }
  }
  std::cout << "Fin" << std::to_string(resp.result.size()) << std::endl;
  resp.controlImage = cv::Mat::zeros(ch1->originalImage.rows, ch1->originalImage.cols, CV_32FC3);
  joda::func::DetectionFunction::paintBoundingBox(resp.controlImage, resp.result, false);
  return resp;
}

}    // namespace joda::pipeline
