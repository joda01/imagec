///
/// \file      exporter.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <sys/types.h>
#include <xlsxwriter/workbook.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"

#include "backend/helper/database/database.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::db {

class BatchExporter
{
public:
  static void startExportHeatmap(const std::map<int32_t, joda::table::Table> &data, const settings::AnalyzeSettings &analyzeSettings,
                                 const std::string &jobName, std::chrono::system_clock::time_point timeStarted,
                                 std::chrono::system_clock::time_point timeFinished, const std::string &outputFileName);
  static void startExportList(const std::map<int32_t, joda::table::Table> &data, const settings::AnalyzeSettings &analyzeSettings,
                              const std::string &jobName, std::chrono::system_clock::time_point timeStarted,
                              std::chrono::system_clock::time_point timeFinished, const std::string &outputFileName);

private:
  /////////////////////////////////////////////////////

  struct WorkBook
  {
    lxw_workbook *workbook;
    lxw_format *header;
    lxw_format *headerInvalid;
    lxw_format *imageHeaderHyperlinkFormat;
    lxw_format *imageHeaderHyperlinkFormatInvalid;
    lxw_format *merge_format;
    lxw_format *headerBold;
    lxw_format *fontNormal;
    lxw_format *numberFormat;
    lxw_format *numberFormatInvalid;
    lxw_format *numberFormatScientific;
    lxw_format *numberFormatInvalidScientific;
  };

  struct Pos
  {
    uint32_t row = 0;
    uint32_t col = 0;
  };
  static WorkBook createWorkBook(std::string outputFileName);
  static void createAnalyzeSettings(WorkBook &, const settings::AnalyzeSettings &settings, const std::string &jobName,
                                    std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished);
  static void createHeatmap(const WorkBook &, std::pair<Pos, lxw_worksheet *> &sheet, const table::Table &data);
  static void createList(const WorkBook &, std::pair<Pos, lxw_worksheet *> &sheet, const table::Table &data);

  static void paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset, lxw_format *header, lxw_format *numberFormat,
                               lxw_format *mergeFormat, const std::string &title);
  static Pos paintHeatmap(const WorkBook &workbookSettings, lxw_worksheet *worksheet, const joda::table::Table &table, uint32_t rowOffset);
};

}    // namespace joda::db
