///
/// \file      exporter_xlsx.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <vector>
#include "../../data/heatmap/data_heatmap.hpp"
#include "../exportable.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::exporter::xlsx {

struct ExportSettings
{
  struct ExportFilter
  {
    int32_t plateId = 0;
    std::string groupName;
    int32_t tStack = 0;
    std::string imageFileName;
  };

  enum class ExportFormat
  {
    XLSX,
    R
  };

  enum class ExportStyle
  {
    LIST,
    HEATMAP
  };

  enum class ExportView
  {
    PLATE,
    WELL,
    IMAGE
  };
  ExportStyle style;
  ExportFormat format;
  ExportView view;
  ExportFilter filter;
};

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class Exporter
{
public:
  /////////////////////////////////////////////////////
  static void startExport(const std::vector<const Exportable *> &data, const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                          std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                          const std::string &unit, const std::string &outputFileName);

  static void startHeatmapExport(const std::vector<const Exportable *> &data, const settings::AnalyzeSettings &analyzeSettings,
                                 const std::string &jobName, std::chrono::system_clock::time_point timeStarted,
                                 std::chrono::system_clock::time_point timeFinished, const std::string &outputFileName,
                                 const settings::ResultsSettings &filterSettings, ExportSettings::ExportView view, int32_t imageHeight,
                                 int32_t imageWidth, const std::string &unit);

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
    lxw_format *idFormat;
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

  /////////////////////////////////////////////////////
  static WorkBook createWorkBook(std::string outputFileName);
  static void createAnalyzeSettings(WorkBook &workbookSettings, const settings::AnalyzeSettings &settings, const std::string &jobName,
                                    std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished);
  static void writeWorkSheet(const Exporter::WorkBook &, const Exportable *, int32_t index, const std::string &unit);

  static std::string prepareSheetName(std::string);

  static Pos paintHeatmap(const WorkBook &workbookSettings, lxw_worksheet *worksheet, const joda::table::Table &table, uint32_t rowOffset);

  static void paintPlateBorder(lxw_worksheet *sheet, uint32_t rows, uint16_t cols, uint32_t rowOffset, lxw_format *header, lxw_format *numberFormat,
                               lxw_format *mergeFormat, const std::string &title);
};

}    // namespace joda::exporter::xlsx
