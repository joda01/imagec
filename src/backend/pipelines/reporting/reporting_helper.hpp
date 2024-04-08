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
    CONFIDENCE                  = 0x0100,
    AREA_SIZE                   = 0x0200,
    PERIMETER                   = 0x0300,
    CIRCULARITY                 = 0x0400,
    VALIDITY                    = 0x0500,
    INVALIDITY                  = 0x0600,
    CENTER_OF_MASS_X            = 0x0700,
    CENTER_OF_MASS_Y            = 0x0800,
    INTENSITY_AVG               = 0x0900,    // Start of the dynamic section
    INTENSITY_MIN               = 0x0A00,
    INTENSITY_MAX               = 0x0B00,
    INTENSITY_AVG_CROSS_CHANNEL = 0x0C00,    // Start of the dynamic section
    INTENSITY_MIN_CROSS_CHANNEL = 0x0D00,
    INTENSITY_MAX_CROSS_CHANNEL = 0x0E00,
    INTERSECTION_CROSS_CHANNEL  = 0x0F00
  };

  static constexpr uint32_t COLUMN_MASK = 0xFFFFFF0;

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
                                    const std::string &channelName, int realChannelIdx, int tempChannelIdx,
                                    const std::set<int32_t> &colocGroup = {});
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
  static inline std::mutex mAppendToAllOverReportMutex;
};

}    // namespace joda::pipeline::reporting
