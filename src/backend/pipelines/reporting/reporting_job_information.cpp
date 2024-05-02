
///
/// \file      reporting_job_information.xlsx.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Write job information
///

#include "reporting_job_information.hpp"
#include <chrono>
#include <string>
#include "backend/helper/helper.hpp"
#include "backend/results/results_container.hpp"
#include "backend/settings/analze_settings.hpp"
#include "version.h"

namespace joda::pipeline::reporting {

void JobInformation::writeReport(const joda::settings::AnalyzeSettings &analyzeSettings,
                                 const std::map<std::string, joda::results::ReportingContainer> &results,
                                 const joda::results::JobMeta &meta, lxw_worksheet *worksheet, lxw_format *header,
                                 lxw_format *fontNormal)
{
  int rowIdx = 0;

  auto writeRow = [&rowIdx, &worksheet, &header, &fontNormal](const std::string &key, const std::string &value) {
    worksheet_write_string(worksheet, rowIdx, 0, key.data(), header);
    worksheet_write_string(worksheet, rowIdx, 1, value.data(), fontNormal);
    rowIdx++;
  };

  auto emptyLine = [&rowIdx](int nr = 1) {
    for(int n = 0; n < nr; n++) {
      rowIdx++;
    }
  };

  worksheet_set_column_pixels(worksheet, 0, 0, 200, NULL);
  worksheet_set_column_pixels(worksheet, 1, 1, 200, NULL);

  writeRow("Version", Version::getVersion());
  writeRow("Build", Version::getBuildTime());
  emptyLine(2);
  writeRow("Job name", meta.jobName);
  writeRow("Job started at", joda::helper::timepointToIsoString(meta.timeStarted));
  writeRow("Job finished at", joda::helper::timepointToIsoString(meta.timeFinished));
  writeRow("Nr. of channels", std::to_string(analyzeSettings.channels.size()));
  emptyLine(2);

  writeRow("Group nr.", std::to_string(results.size()));
  int64_t nrOfImages = 0;
  for(const auto &[_, group] : results) {
    for(const auto &[_, tb] : group.mColumns) {
      nrOfImages = tb.getNrOfRows();
    }
  }
  writeRow("Image nr.", std::to_string(nrOfImages));
  emptyLine(2);

  for(const auto &channel : analyzeSettings.channels) {
    writeRow("Name", channel.meta.name);
    writeRow("Index", joda::settings::to_string(channel.meta.channelIdx));
    writeRow("Type", std::to_string((int) channel.meta.type));
    writeRow("Detection mode", std::to_string((int) channel.detection.detectionMode));
    writeRow("Threshold", std::to_string((int) channel.detection.threshold.mode));
    writeRow("Threshold min", std::to_string(channel.detection.threshold.thresholdMin));

    emptyLine(1);
  }
}

}    // namespace joda::pipeline::reporting
