
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
#include <string>
#include "version.h"

namespace joda::pipeline::reporting {

void JobInformation::writeReport(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                 const std::map<std::string, joda::results::ReportingContainer> &results,
                                 const std::string &jobName, lxw_worksheet *worksheet, lxw_format *header,
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
  writeRow("Job name", jobName);
  writeRow("Nr. of channels", std::to_string(analyzeSettings.getChannelsVector().size()));
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

  for(const auto &[_, channel] : analyzeSettings.getChannels()) {
    writeRow("Name", channel.getChannelInfo().getName());
    writeRow("Index", std::to_string(channel.getChannelInfo().getChannelIndex()));
    writeRow("Type", channel.getChannelInfo().getTypeString());
    writeRow("Detection mode", channel.getDetectionSettings().getDetectionModeString());
    writeRow("Threshold", channel.getDetectionSettings().getThersholdSettings().getThresholdString());
    writeRow("Threshold min", std::to_string(channel.getDetectionSettings().getThersholdSettings().getThresholdMin()));

    emptyLine(1);
  }
}

}    // namespace joda::pipeline::reporting
