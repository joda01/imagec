///
/// \file      reporting_container.cpp
/// \author    Joachim Danmayr
/// \date      2024-03-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "results_container.hpp"
#include <xlsxwriter/format.h>
#include "backend/pipelines/reporting/reporting_details.xlsx.hpp"
#include "backend/pipelines/reporting/reporting_job_information.hpp"
#include "backend/pipelines/reporting/reporting_overview_xlsx.hpp"
#include "xlsxwriter.h"

namespace joda::results {

ReportingContainer::ReportingContainer()
{
}

void ReportingContainer::flushReportToFile(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                           const std::map<std::string, ReportingContainer> &containers,
                                           const std::string &fileName, const JobMeta &meta, OutputFormat format,
                                           bool writeRunMeta)
{
  lxw_workbook *workbook = workbook_new(fileName.data());

  /* Add a format. */
  lxw_format *header = workbook_add_format(workbook);
  format_set_bold(header);
  format_set_pattern(header, LXW_PATTERN_SOLID);
  format_set_bg_color(header, 0x002242);
  format_set_font_color(header, 0xFFFFFF);
  format_set_border(header, LXW_BORDER_THIN);
  format_set_font_size(header, 10);

  lxw_format *imageHeaderHyperlinkFormat = workbook_add_format(workbook);
  format_set_bold(imageHeaderHyperlinkFormat);
  format_set_pattern(imageHeaderHyperlinkFormat, LXW_PATTERN_SOLID);
  format_set_bg_color(imageHeaderHyperlinkFormat, 0x002242);
  format_set_font_color(imageHeaderHyperlinkFormat, 0xFFFFFF);
  format_set_border(imageHeaderHyperlinkFormat, LXW_BORDER_THIN);
  format_set_underline(imageHeaderHyperlinkFormat, LXW_UNDERLINE_SINGLE);
  format_set_font_size(imageHeaderHyperlinkFormat, 10);

  // Define the cell format for the merged cells.
  lxw_format *merge_format = workbook_add_format(workbook);
  format_set_align(merge_format, LXW_ALIGN_CENTER);
  format_set_align(merge_format, LXW_ALIGN_VERTICAL_CENTER);
  format_set_bold(merge_format);
  format_set_pattern(merge_format, LXW_PATTERN_SOLID);
  format_set_bg_color(merge_format, 0x002242);
  format_set_font_color(merge_format, 0xFFFFFF);
  format_set_border(merge_format, LXW_BORDER_THIN);
  format_set_font_size(merge_format, 10);

  lxw_format *headerBold = workbook_add_format(workbook);
  format_set_bold(headerBold);
  format_set_pattern(headerBold, LXW_PATTERN_SOLID);
  format_set_bg_color(headerBold, 0xFFFFFF);
  format_set_font_color(headerBold, 0x000000);
  format_set_border(headerBold, LXW_BORDER_THIN);
  format_set_bold(headerBold);
  format_set_font_size(headerBold, 10);

  lxw_format *fontNormal = workbook_add_format(workbook);
  format_set_font_size(fontNormal, 10);
  format_set_border(fontNormal, LXW_BORDER_THIN);
  format_set_pattern(fontNormal, LXW_PATTERN_SOLID);
  format_set_bg_color(fontNormal, 0xFFFFFF);
  format_set_font_color(fontNormal, 0x000000);

  // Number format
  lxw_format *numberFormat = workbook_add_format(workbook);
  format_set_num_format(numberFormat, "0.00");
  format_set_font_size(numberFormat, 10);

  if(writeRunMeta) {
    // Write run meta information
    lxw_worksheet *worksheetMeta = workbook_add_worksheet(workbook, "Job info");
    joda::pipeline::reporting::JobInformation::writeReport(analyzeSettings, containers, meta, worksheetMeta, headerBold,
                                                           fontNormal);
  }

  int colOffsetIn          = 0;
  int rowOffsetIn          = 0;
  int rowOffsetStart       = 0;
  lxw_worksheet *worksheet = workbook_add_worksheet(workbook, "Results");
  for(const auto &[folderName, table] : containers) {
    for(const auto &[idx, table] : table.mColumns) {
      // colOffset = table.flushReportToFileXlsx(colOffset, worksheet, header, merge_format);
      if(OutputFormat::HORIZONTAL == format) {
        auto [colOffset, rowOffset] = joda::pipeline::reporting::OverviewReport::writeReport(
            table, folderName, meta.jobName, colOffsetIn, rowOffsetIn, rowOffsetStart, worksheet, header, merge_format,
            numberFormat, imageHeaderHyperlinkFormat);
        colOffsetIn = colOffset;
        rowOffsetIn = rowOffset;
      }
      if(OutputFormat::VERTICAL == format) {
        auto [colOffset, rowOffset] = joda::pipeline::reporting::DetailReport::writeReport(
            table, colOffsetIn, rowOffsetIn, worksheet, header, merge_format, numberFormat);
        colOffsetIn += colOffset + 1;
        rowOffsetIn += rowOffset;
      }
    }
    if(OutputFormat::HORIZONTAL == format) {
      rowOffsetStart = rowOffsetIn += 2;
      rowOffsetIn    = rowOffsetStart;
    }
  }

  workbook_close(workbook);
}

}    // namespace joda::results