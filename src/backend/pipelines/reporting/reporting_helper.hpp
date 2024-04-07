///
/// \file      reporting_helper.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Reporting helper
///

#pragma once

#include "backend/image_reader/image_reader.hpp"
#include "backend/results/results_container.hpp"
#include "backend/settings/analze_settings_parser.hpp"

namespace joda::pipeline::reporting {

class Helper
{
public:
  /////////////////////////////////////////////////////
  enum class ColumnIndexDetailedReport : int
  {
    CONFIDENCE       = 0,
    AREA_SIZE        = 1,
    PERIMETER        = 2,
    CIRCULARITY      = 3,
    VALIDITY         = 4,
    CENTER_OF_MASS_X = 5,
    CENTER_OF_MASS_Y = 6,
    DYNAMIC          = 7,    // Start of the dynamic section
    DYNAMIC_MIN      = 8,
    DYNAMIC_MAX      = 9,
  };

  struct RegexResult
  {
    std::string group;
    int32_t row = -1;
    int32_t col = -1;
    int32_t img = -1;
  };

  static RegexResult applyRegex(const std::string &regex, const std::string &fileName);
  static RegexResult applyGroupRegex(const std::string &fileName);

  static void setDetailReportHeader(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                    joda::results::ReportingContainer &detailReportTable,
                                    const std::string &channelName, int tempChannelIdx);
  static void appendToDetailReport(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                   const joda::func::DetectionResponse &result,
                                   joda::results::ReportingContainer &detailReportTable,
                                   const std::string &detailReportOutputPath, const std::string &jobName,
                                   int realChannelIdx, int tempChannelIdx, uint32_t tileIdx,
                                   const ImageProperties &imgProps);
  static void appendToAllOverReport(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                    std::map<std::string, joda::results::ReportingContainer> &allOverReport,
                                    const joda::results::ReportingContainer &detailedReport,
                                    const std::string &imagePath, const std::string &imageName, int nrOfChannels);

private:
  /////////////////////////////////////////////////////
  static auto stringToNumber(const std::string &str) -> int;
  static auto getGroupToStoreImageIn(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                     const std::string &imagePath, const std::string &imageName) -> std::string;

  /////////////////////////////////////////////////////
};

}    // namespace joda::pipeline::reporting
