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
#include "duration_count/duration_count.h"

namespace joda::pipeline {

///
/// \brief      Counts the number of nucleus in an image
/// \author     Joachim Danmayr
/// \param[in]  img     Image to analyze
///
void NucleusCounter::analyzeImage(const joda::Image &img)
{
  auto i                = DurationCount::start("detect");
  auto enhancedContrast = img.mImage *= 10;
  ai::ObjectDetector obj("/workspaces/open-bio-image-processor/test/best.onnx", {"nuclues", "nucleus_no_focus"});
  ai::DetectionResults result = obj.forward(enhancedContrast);
  DurationCount::stop(i);

  i = DurationCount::start("write");
  writeReport(result, img.mName);
  DurationCount::stop(i);

  i = DurationCount::start("ctrl");
  obj.paintBoundingBox(enhancedContrast, result);
  cv::imwrite(getOutputFolder() + "/" + img.mName + ".jpg", enhancedContrast);
  DurationCount::stop(i);
}

///
/// \brief      Writes detection reporting
/// \author     Joachim Danmayr
/// \param[in]  Outcome of the AI prediction
///
void NucleusCounter::writeReport(const ai::DetectionResults &prediction, const std::string imgName)
{
  reporting::Table imageReport;

  imageReport.setColumnNames({{0, "Index"}, {1, "Class"}, {2, "Probability"}});

  int nrOfNucleus = 0;
  for(const auto &pred : prediction) {
    imageReport.appendValueToColumn(0, pred.index);
    imageReport.appendValueToColumn(1, pred.classId);
    imageReport.appendValueToColumn(2, pred.confidence);
    nrOfNucleus++;
  }
  std::lock_guard<std::mutex> lockGuard(mWriteMutex);
  std::string fileName = getOutputFolder() + "/" + imgName + ".csv";
  imageReport.flushReportToFile(fileName);

  reporting()->appendValueToColumn(0, nrOfNucleus);
}

}    // namespace joda::pipeline
