///
/// \file      reporting.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "backend/reporting/reporting_container.hpp"
#include "backend/settings/analze_settings_parser.hpp"

namespace joda::pipeline {

///
/// \class      Reporting generation
/// \author     Joachim Danmayr
/// \brief
///
class Reporting
{
public:
  /////////////////////////////////////////////////////
  struct RegexResult
  {
    std::string group;
    int32_t row = -1;
    int32_t col = -1;
  };

  Reporting(const joda::settings::json::AnalyzeSettings &);

  void setDetailReportHeader(joda::reporting::ReportingContainer &detailReportTable, const std::string &channelName,
                             int tempChannelIdx);
  void appendToDetailReport(joda::func::DetectionResponse &result,
                            joda::reporting::ReportingContainer &detailReportTable,
                            const std::string &detailReportOutputPath, int tempChannelIdx, uint32_t tileIdx);
  void appendToAllOverReport(std::map<std::string, joda::reporting::ReportingContainer> &allOverReport,
                             const joda::reporting::ReportingContainer &detailedReport, const std::string &imagePath,
                             const std::string &imageName, int nrOfChannels);

  auto getGroupToStoreImageIn(const std::string &imagePath, const std::string &imageName) -> std::string;

  void createAllOverHeatMap(std::map<std::string, joda::reporting::ReportingContainer> &allOverReport,
                            const std::string &fileName);

  static RegexResult applyRegex(const std::string &regex, const std::string &fileName);

private:
  /////////////////////////////////////////////////////
  enum class ColumnIndexDetailedReport : int
  {
    CONFIDENCE    = 0,
    AREA_SIZE     = 1,
    PERIMETER     = 2,
    CIRCULARITY   = 3,
    VALIDITY      = 4,
    INTENSITY     = 5,
    INTENSITY_MIN = 6,
    INTENSITY_MAX = 7,
  };

  const joda::settings::json::AnalyzeSettings &mAnalyzeSettings;
};

}    // namespace joda::pipeline
