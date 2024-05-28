///
/// \file      exporter_xlsx.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     XLSX exporter
///

#include "exporter_xlsx.hpp"
#include <xlsxwriter/format.h>
#include "xlsxwriter.h"

namespace joda::results::exporter {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ExporterXlsx::exportAsList(const joda::results::Table &table, std::string outputFileName)
{
  auto workbookSettings    = prepare(outputFileName);
  lxw_worksheet *worksheet = workbook_add_worksheet(workbookSettings.workbook, "results");

  for(int n = 0; n < table.getColHeaderSize(); n++) {
    worksheet_write_string(worksheet, 0, n + 1, table.getColHeader(n).data(), workbookSettings.header);
  }

  for(int n = 0; n < table.getRowHeaderSize(); n++) {
    worksheet_write_string(worksheet, 1 + n, 0, table.getRowHeader(n).data(), workbookSettings.header);
  }

  for(int row = 0; row < table.getRows(); row++) {
    for(int col = 0; col < table.getCols(); col++) {
      worksheet_write_number(worksheet, 1 + row, 1 + col, table.data(row, col).getVal(), workbookSettings.numberFormat);
    }
  }

  workbook_close(workbookSettings.workbook);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ExporterXlsx::exportAsHeatmap(const joda::results::Table &table, std::string outputFileName)
{
  auto workbookSettings    = prepare(outputFileName);
  lxw_worksheet *worksheet = workbook_add_worksheet(workbookSettings.workbook, "results");
  paintPlateBorder(worksheet, table.getRows(), table.getCols(), 0, workbookSettings.header,
                   workbookSettings.numberFormatScientific);

  for(int row = 0; row < table.getRows(); row++) {
    for(int col = 0; col < table.getCols(); col++) {
      auto *format = workbookSettings.numberFormatScientific;
      if(!table.data(row, col).isValid()) {
        format = workbookSettings.numberFormatInvalidScientific;
      }
      worksheet_write_number(worksheet, 1 + row, 1 + col, table.data(row, col).getVal(), format);
    }
  }
  workbook_close(workbookSettings.workbook);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
ExporterXlsx::WorkBook ExporterXlsx::prepare(std::string outputFileName)
{
  if(!outputFileName.ends_with(".xlsx")) {
    outputFileName += ".xlsx";
  }
  lxw_workbook *workbook = workbook_new(outputFileName.data());

  /* Add a format. */
  lxw_format *header = workbook_add_format(workbook);
  format_set_bold(header);
  format_set_pattern(header, LXW_PATTERN_SOLID);
  format_set_bg_color(header, 0x002242);
  format_set_font_color(header, 0xFFFFFF);
  format_set_border(header, LXW_BORDER_THIN);
  format_set_font_size(header, 10);

  // Had for cols with invalid data
  lxw_format *headerInvalid = workbook_add_format(workbook);
  format_set_bold(headerInvalid);
  format_set_pattern(headerInvalid, LXW_PATTERN_SOLID);
  format_set_bg_color(headerInvalid, 0x820000);
  format_set_font_color(headerInvalid, 0xFFFFFF);
  format_set_border(headerInvalid, LXW_BORDER_THIN);
  format_set_font_size(headerInvalid, 10);

  //
  lxw_format *imageHeaderHyperlinkFormat = workbook_add_format(workbook);
  format_set_bold(imageHeaderHyperlinkFormat);
  format_set_pattern(imageHeaderHyperlinkFormat, LXW_PATTERN_SOLID);
  format_set_bg_color(imageHeaderHyperlinkFormat, 0x002242);
  format_set_font_color(imageHeaderHyperlinkFormat, 0xFFFFFF);
  format_set_border(imageHeaderHyperlinkFormat, LXW_BORDER_THIN);
  format_set_underline(imageHeaderHyperlinkFormat, LXW_UNDERLINE_SINGLE);
  format_set_font_size(imageHeaderHyperlinkFormat, 10);

  //
  lxw_format *imageHeaderHyperlinkFormatInvalid = workbook_add_format(workbook);
  format_set_bold(imageHeaderHyperlinkFormatInvalid);
  format_set_pattern(imageHeaderHyperlinkFormatInvalid, LXW_PATTERN_SOLID);
  format_set_bg_color(imageHeaderHyperlinkFormatInvalid, 0x820000);
  format_set_font_color(imageHeaderHyperlinkFormatInvalid, 0xFFFFFF);
  format_set_border(imageHeaderHyperlinkFormatInvalid, LXW_BORDER_THIN);
  format_set_underline(imageHeaderHyperlinkFormatInvalid, LXW_UNDERLINE_SINGLE);
  format_set_font_size(imageHeaderHyperlinkFormatInvalid, 10);

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

  // Number format invalid
  lxw_format *numberFormatInvalid = workbook_add_format(workbook);
  format_set_num_format(numberFormatInvalid, "0.00");
  format_set_font_size(numberFormatInvalid, 10);
  format_set_font_color(numberFormatInvalid, 0x820000);

  // Number format scientific
  lxw_format *numberFormatScientific = workbook_add_format(workbook);
  format_set_num_format(numberFormatScientific, "0.00E+00");
  format_set_font_size(numberFormatScientific, 10);
  format_set_align(numberFormatScientific, LXW_ALIGN_CENTER);
  format_set_align(numberFormatScientific, LXW_ALIGN_VERTICAL_CENTER);

  // Number format invalid scientific
  lxw_format *numberFormatInvalidScientific = workbook_add_format(workbook);
  numberFormatInvalidScientific             = workbook_add_format(workbook);
  format_set_num_format(numberFormatInvalidScientific, "0.00E+00");
  format_set_font_size(numberFormatInvalidScientific, 10);
  format_set_align(numberFormatInvalidScientific, LXW_ALIGN_CENTER);
  format_set_align(numberFormatInvalidScientific, LXW_ALIGN_VERTICAL_CENTER);
  // format_set_border(numberFormatInvalid, LXW_BORDER_THIN);                 // Set border style to thin
  format_set_diag_type(numberFormatInvalidScientific, LXW_DIAGONAL_BORDER_UP_DOWN);

  return WorkBook{workbook,
                  header,
                  headerInvalid,
                  imageHeaderHyperlinkFormat,
                  imageHeaderHyperlinkFormatInvalid,
                  merge_format,
                  headerBold,
                  fontNormal,
                  numberFormat,
                  numberFormatInvalid,
                  numberFormatScientific,
                  numberFormatInvalidScientific};
}

///
/// \brief      Paint the borders of the heatmap
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ExporterXlsx::paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset,
                                    lxw_format *header, lxw_format *numberFormat)
{
  const int32_t HEADER_CELL_SIZE = 15;

  ///////////////////////////7
  lxw_conditional_format *condFormat = new lxw_conditional_format();
  condFormat->type                   = LXW_CONDITIONAL_3_COLOR_SCALE;
  condFormat->format                 = numberFormat;
  condFormat->min_color              = 0x63BE7B;
  condFormat->min_rule_type          = LXW_CONDITIONAL_RULE_TYPE_MINIMUM;
  condFormat->mid_color              = 0xFFEB84;
  condFormat->mid_rule_type          = LXW_CONDITIONAL_RULE_TYPE_PERCENTILE;
  condFormat->mid_value              = 50;
  condFormat->max_color              = 0xF8696B;
  condFormat->max_rule_type          = LXW_CONDITIONAL_RULE_TYPE_MAXIMUM;
  ///////////////////

  // Column
  worksheet_set_column_pixels(sheet, 0, 0, HEADER_CELL_SIZE, NULL);
  worksheet_set_column_pixels(sheet, cols + 1, cols + 1, HEADER_CELL_SIZE, NULL);

  for(int col = 1; col < cols + 1; col++) {
    worksheet_set_column_pixels(sheet, col, col, CELL_SIZE, NULL);
    worksheet_write_string(sheet, rowOffset, col, std::to_string(col).data(), header);
    worksheet_write_string(sheet, rows + rowOffset + 1, col, std::to_string(col).data(), header);
  }

  // Row
  worksheet_set_row_pixels(sheet, rowOffset, HEADER_CELL_SIZE, NULL);
  for(int row = 1; row < rows + 1; row++) {
    char toWrt[2];
    toWrt[0] = (row - 1) + 'A';
    toWrt[1] = 0;

    worksheet_set_row_pixels(sheet, row + rowOffset, CELL_SIZE, NULL);
    worksheet_write_string(sheet, row + rowOffset, 0, toWrt, header);
    worksheet_write_string(sheet, row + rowOffset, cols + 1, toWrt, header);
  }

  worksheet_conditional_format_range(sheet, 1 + rowOffset, 1, 1 + rowOffset + rows, 1 + cols, condFormat);
}

}    // namespace joda::results::exporter
