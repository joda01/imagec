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

#include <map>
#include <mutex>
#include <set>
#include "backend/image_reader/image_reader.hpp"
#include "backend/results/results.hpp"
#include "backend/settings/channel/channel_index.hpp"

namespace joda::settings {
class AnalyzeSettings;
}

namespace joda::results {
class TableWorkbook;
}

namespace joda::func {
class DetectionResponse;
}

namespace joda::results {

class Helper
{
public:
  struct RegexResult
  {
    std::string group;
    int32_t row = -1;
    int32_t col = -1;
    int32_t img = -1;
  };

  static RegexResult applyRegex(const std::string &regex, const std::string &fileName);
  static RegexResult applyGroupRegex(const std::string &fileName);

  static void setDetailReportHeader(const joda::settings::AnalyzeSettings &analyzeSettings,
                                    joda::results::WorkSheet &detailReportTable, const std::string &channelName,
                                    joda::settings::ChannelIndex chIdx);

  static void appendToDetailReport(const joda::settings::AnalyzeSettings &analyzeSettings,
                                   const joda::func::DetectionResponse &result,
                                   joda::results::WorkSheet &detailReportTable,
                                   const std::string &detailReportOutputPath, const std::string &jobName,
                                   joda::settings::ChannelIndex realChannelIdx, uint32_t tileIdx,
                                   const ImageProperties &imgProps, const std::string &imagePath);

  static void appendToAllOverReport(const joda::settings::AnalyzeSettings &analyzeSettings,
                                    joda::results::WorkSheet &allOverReport,
                                    const joda::results::WorkSheet &detailedReport, const std::string &imagePath,
                                    const std::string &imageName, int nrOfChannels);

private:
  /////////////////////////////////////////////////////
  static auto stringToNumber(const std::string &str) -> int;
  static auto getGroupToStoreImageIn(const joda::settings::AnalyzeSettings &analyzeSettings,
                                     const std::string &imagePath, const std::string &imageName) -> std::string;

  /////////////////////////////////////////////////////
  static inline std::mutex mAppendToAllOverReportMutex;
};

}    // namespace joda::results
