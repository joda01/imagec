///
/// \file      nucleus_count.cpp
/// \author    Joachim Danmayr
/// \date      2023-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Pipeline which implements an AI based nuclues counter
///

#include "nucleus_count.hpp"
#include <fstream>
#include <iostream>
#include <mutex>

namespace joda::pipeline {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
void NucleusCounter::analyzeImage(const joda::Image &img)
{
  auto enhancedContrast = img.mImage *= 10;
  ai::ObjectDetector obj("/workspaces/open-bio-image-processor/test/best.onnx", {"nuclues", "nucleus_no_focus"});
  ai::DetectionResults result = obj.forward(enhancedContrast);
  writeReport(result, img.mName);
  obj.paintBoundingBox(enhancedContrast, result);
  cv::imwrite(getOutputFolder() + "/" + img.mName + ".jpg", enhancedContrast);
}

///
/// \brief      Writes detection reporting
/// \author     Joachim Danmayr
/// \param[in]  Outcome of the AI prediction
///
void NucleusCounter::writeReport(const ai::DetectionResults &prediction, const std::string imgName)
{
  std::string detailedReportBuffer = "Index\tClass ID\tProbability\n";
  int idx                          = 0;
  for(const auto &pred : prediction) {
    detailedReportBuffer += std::to_string(pred.index);
    detailedReportBuffer += std::to_string(pred.classId);
    detailedReportBuffer += std::to_string(pred.confidence);
    idx++;
  }
  std::lock_guard<std::mutex> lockGuard(mWriteMutex);
  reporting()->counter += idx;
  std::string fileName = getOutputFolder() + "/" + imgName + ".csv";
  std::ofstream detailedReport;
  detailedReport.open(fileName);
  detailedReport << detailedReportBuffer;
  detailedReport.close();

  reporting()->counter++;
}

}    // namespace joda::pipeline
