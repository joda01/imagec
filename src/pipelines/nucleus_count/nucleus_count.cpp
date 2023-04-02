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
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string_view>
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
  writeReport(result, img);
  DurationCount::stop(i);

  i = DurationCount::start("ctrl");
  obj.paintBoundingBox(enhancedContrast, result);
  cv::imwrite(getOutputFolder() + "/" + img.mName + "/" + img.getUniqueName() + ".jpg", enhancedContrast);
  DurationCount::stop(i);
}

///
/// \brief      Writes detection reporting
/// \author     Joachim Danmayr
/// \param[in]  Outcome of the AI prediction
///
void NucleusCounter::writeReport(const ai::DetectionResults &prediction, const joda::Image &img)
{
  reporting::Table imageReport;

  imageReport.setColumnNames({{0, "Index"}, {1, "Class"}, {2, "Probability [0-1]"}, {3, "Diameter [px]"}});

  int nrOfNucleus = 0;
  for(const auto &pred : prediction) {
    float diameter = float(pred.box.height + pred.box.width) / 2.0f;
    imageReport.appendValueToColumn(0, pred.index);
    imageReport.appendValueToColumn(1, pred.classId);
    imageReport.appendValueToColumn(2, pred.confidence);
    imageReport.appendValueToColumn(3, diameter);
    nrOfNucleus++;
  }
  std::lock_guard<std::mutex> lockGuard(mWriteMutex);
  std::string outPath = getOutputFolder() + "/" + img.mName;
  if(!std::filesystem::exists(outPath)) {
    std::filesystem::create_directories(outPath);
  }

  imageReport.flushReportToFile(outPath + "/" + img.getUniqueName() + ".csv");

  mergeReportInt(img.getUniqueName(), *reporting(), imageReport);
}

void NucleusCounter::mergeReportInt(const std::string &rowName, joda::reporting::Table &mergeTo,
                                    const joda::reporting::Table &mergeFrom)
{
  mergeTo.setColumnNames({{0, "Count"}, {1, "Diameter [px]"}});
  if(mergeFrom.getStatisitcs().contains(0)) {
    mergeTo.appendValueToColumn(rowName, 0, mergeFrom.getStatisitcs().at(0).getNr());
  } else {
    mergeTo.appendValueToColumn(rowName, 0, 0);
  }
  if(mergeFrom.getStatisitcs().contains(3)) {
    mergeTo.appendValueToColumn(rowName, 1, mergeFrom.getStatisitcs().at(3).getMean());
  } else {
    mergeTo.appendValueToColumn(rowName, 1, 0);
  }
}

void NucleusCounter::mergeReport(const std::string &rowName, joda::reporting::Table &mergeTo,
                                 const joda::reporting::Table &mergeFrom)
{
  mergeTo.setColumnNames({{0, "Count"}, {1, "Diameter [px]"}});
  if(mergeFrom.getStatisitcs().contains(0)) {
    mergeTo.appendValueToColumn(rowName, 0, mergeFrom.getStatisitcs().at(0).getSum());
  } else {
    mergeTo.appendValueToColumn(rowName, 0, 0);
  }
  if(mergeFrom.getStatisitcs().contains(3)) {
    mergeTo.appendValueToColumn(rowName, 1, mergeFrom.getStatisitcs().at(3).getMean());
  } else {
    mergeTo.appendValueToColumn(rowName, 1, 0);
  }
}

}    // namespace joda::pipeline
