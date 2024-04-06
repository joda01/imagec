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

#include "backend/image_reader/image_reader.hpp"
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
    int32_t img = -1;
  };

  Reporting(const joda::settings::json::AnalyzeSettings &);

  void setDetailReportHeader(joda::reporting::ReportingContainer &detailReportTable, const std::string &channelName,
                             int tempChannelIdx);
  void appendToDetailReport(const joda::func::DetectionResponse &result,
                            joda::reporting::ReportingContainer &detailReportTable,
                            const std::string &detailReportOutputPath, const std::string &jobName, int realChannelIdx,
                            int tempChannelIdx, uint32_t tileIdx, const ImageProperties &imgProps);
  void appendToAllOverReport(std::map<std::string, joda::reporting::ReportingContainer> &allOverReport,
                             const joda::reporting::ReportingContainer &detailedReport, const std::string &imagePath,
                             const std::string &imageName, int nrOfChannels);

  auto getGroupToStoreImageIn(const std::string &imagePath, const std::string &imageName) -> std::string;

  void createHeatMapForImage(const joda::reporting::ReportingContainer &containers, int64_t imageWidth,
                             int64_t imageHeight, const std::string &fileName);
  void createAllOverHeatMap(std::map<std::string, joda::reporting::ReportingContainer> &allOverReport,
                            const std::string &fileName);

  void createHeatmapOfWellsForGroup(lxw_workbook *workbook, const std::string &groupName,
                                    const joda::reporting::ReportingContainer &groupReports, lxw_format *header,
                                    lxw_format *numberFormat);

  static RegexResult applyRegex(const std::string &regex, const std::string &fileName);

private:
  static auto stringToNumber(const std::string &str) -> int;
  static RegexResult applyGroupRegex(const std::string &fileName);

  static constexpr int32_t CELL_SIZE = 60;

  void paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset, lxw_format *header,
                        lxw_format *numberFormat) const;

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

  const joda::settings::json::AnalyzeSettings &mAnalyzeSettings;
  std::mutex mAppendMutex;
};

}    // namespace joda::pipeline
