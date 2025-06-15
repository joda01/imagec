

#include "exporter.hpp"
#include <xlsxwriter/worksheet.h>
#include <cstddef>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"

#include "backend/enums/enums_grouping.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::startExportHeatmap(const std::map<int32_t, joda::table::Table> &data, const settings::AnalyzeSettings &analyzeSettings,
                                       const std::string &jobName, std::chrono::system_clock::time_point timeStarted,
                                       std::chrono::system_clock::time_point timeFinished, const std::string &outputFileName)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  auto workbookSettings = createWorkBook(outputFileName);

  createAnalyzeSettings(workbookSettings, analyzeSettings, jobName, timeStarted, timeFinished);

  std::map<std::string, std::pair<Pos, lxw_worksheet *>> sheets;

  for(const auto &[_, table] : data) {
    std::string name = table.getMeta().className;    /// \todo Was cluster
    // Max. sheet name length = 31 because of excel limitation
    if(name.size() > 30) {
      name = name.substr(0, 30);
    }
    name.erase(std::remove_if(name.begin(), name.end(), [](char c) { return !std::isalnum(static_cast<unsigned char>(c)); }), name.end());
    if(!sheets.contains(name)) {
      sheets.emplace(name, std::pair<Pos, lxw_worksheet *>{Pos{}, workbook_add_worksheet(workbookSettings.workbook, name.data())});
    }
    createHeatmap(workbookSettings, sheets.at(name), table);
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
void BatchExporter::startExportList(const joda::table::Table &data, const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                                    std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                                    const std::string &outputFileName)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  auto workbookSettings = createWorkBook(outputFileName);

  createAnalyzeSettings(workbookSettings, analyzeSettings, jobName, timeStarted, timeFinished);

  std::map<std::string, std::pair<Pos, lxw_worksheet *>> sheets;

  std::string sheetName = "Sheet_" + std::to_string(0);
  if(!sheets.contains(sheetName)) {
    sheets.emplace(sheetName, std::pair<Pos, lxw_worksheet *>{Pos{}, workbook_add_worksheet(workbookSettings.workbook, sheetName.data())});
  }
  createList(workbookSettings, sheets.at(sheetName), data);

  workbook_close(workbookSettings.workbook);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::createHeatmap(const WorkBook &workbookSettings, std::pair<Pos, lxw_worksheet *> &sheet, const table::Table &data)
{
  paintPlateBorder(sheet.second, data.getRows(), data.getCols(), sheet.first.row, workbookSettings.header, workbookSettings.merge_format,
                   workbookSettings.numberFormat, data.getTitle());
  sheet.first = paintHeatmap(workbookSettings, sheet.second, data, sheet.first.row);
  sheet.first.row += 2;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::createList(const WorkBook &workbookSettings, std::pair<Pos, lxw_worksheet *> &sheet, const table::Table &data)
{
  int xOffset = sheet.first.col;

  for(int n = 0; n < data.getColHeaderSize(); n++) {
    worksheet_write_string(sheet.second, 0, n + 1 + xOffset, data.getColHeader(n).data(), workbookSettings.header);
  }

  for(int n = 0; n < data.getRowHeaderSize(); n++) {
    worksheet_write_string(sheet.second, n + 1 + xOffset, 0, data.getRowHeader(n).data(), workbookSettings.header);
  }

  for(int row = 0; row < data.getRows(); row++) {
    for(int col = 0; col < data.getCols(); col++) {
      if(data.data(row, col).isValid()) {
        if(data.data(row, col).isNAN()) {
          worksheet_write_blank(sheet.second, row + 1 + xOffset, 1 + col, workbookSettings.numberFormat);
        } else {
          worksheet_write_number(sheet.second, row + 1 + xOffset, 1 + col, data.data(row, col).getVal(), workbookSettings.numberFormat);
        }
      } else {
        if(data.data(row, col).isNAN()) {
          worksheet_write_blank(sheet.second, row + 1 + xOffset, 1 + col, workbookSettings.numberFormatInvalid);
        } else {
          worksheet_write_number(sheet.second, row + 1 + xOffset, 1 + col, data.data(row, col).getVal(), workbookSettings.numberFormatInvalid);
        }
      }
    }
  }

  sheet.first.col = xOffset + data.getCols();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
BatchExporter::WorkBook BatchExporter::createWorkBook(std::string outputFileName)
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
  format_set_font_strikeout(numberFormatInvalid);

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

  return WorkBook{
      workbook,   header,       headerInvalid,       imageHeaderHyperlinkFormat, imageHeaderHyperlinkFormatInvalid, merge_format, headerBold,
      fontNormal, numberFormat, numberFormatInvalid, numberFormatScientific,     numberFormatInvalidScientific};
}

///
/// \brief      Paint the borders of the heatmap
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset, lxw_format *header,
                                     lxw_format *numberFormat, lxw_format *mergeFormat, const std::string &title)
{
  const int32_t ROW_OFFSET       = 2;
  const int32_t HEADER_CELL_SIZE = 15;
  const int32_t CELL_SIZE        = 60;

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

  // Title
  worksheet_merge_range(sheet, rowOffset, 1, rowOffset, cols, "-", mergeFormat);
  worksheet_write_string(sheet, rowOffset, 1, title.data(), header);

  // Column
  worksheet_set_column_pixels(sheet, 0, 0, HEADER_CELL_SIZE, NULL);
  worksheet_set_column_pixels(sheet, cols + 1, cols + 1, HEADER_CELL_SIZE, NULL);

  for(int col = 1; col < cols + 1; col++) {
    worksheet_set_column_pixels(sheet, col, col, CELL_SIZE, NULL);
    worksheet_write_string(sheet, rowOffset + 1, col, std::to_string(col).data(), header);
    worksheet_write_string(sheet, rows + rowOffset + 2, col, std::to_string(col).data(), header);
  }

  // Row
  worksheet_set_row_pixels(sheet, rowOffset + ROW_OFFSET, HEADER_CELL_SIZE, NULL);
  for(int row = 0; row < rows; row++) {
    char toWrt[2];
    toWrt[0] = (row) + 'A';
    toWrt[1] = 0;

    worksheet_set_row_pixels(sheet, row + rowOffset + ROW_OFFSET, CELL_SIZE, NULL);
    worksheet_write_string(sheet, row + rowOffset + ROW_OFFSET, 0, toWrt, header);
    worksheet_write_string(sheet, row + rowOffset + ROW_OFFSET, cols + 1, toWrt, header);
  }

  worksheet_conditional_format_range(sheet, rowOffset + ROW_OFFSET, 1, rowOffset + rows + ROW_OFFSET, 1 + cols, condFormat);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
BatchExporter::Pos BatchExporter::paintHeatmap(const WorkBook &workbookSettings, lxw_worksheet *worksheet, const joda::table::Table &table,
                                               uint32_t rowOffset)
{
  const int32_t ROW_OFFSET = 2;

  for(int row = 0; row < table.getRows(); row++) {
    for(int col = 0; col < table.getCols(); col++) {
      auto *format = workbookSettings.numberFormatScientific;
      if(!table.data(row, col).isValid()) {
        format = workbookSettings.numberFormatInvalidScientific;
      }
      // Offset 2 because of title and plate numbering
      if(table.data(row, col).isNAN()) {
        worksheet_write_blank(worksheet, ROW_OFFSET + row + rowOffset, 1 + col, format);
      } else {
        worksheet_write_number(worksheet, ROW_OFFSET + row + rowOffset, 1 + col, table.data(row, col).getVal(), format);
      }
    }
  }

  return BatchExporter::Pos{.row = table.getRows() + rowOffset + ROW_OFFSET, .col = table.getCols()};
}

///
/// \brief      Add analyze settings to excel
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::createAnalyzeSettings(WorkBook &workbookSettings, const settings::AnalyzeSettings &settings, const std::string &jobName,
                                          std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished)
{
  auto *sheet = workbook_add_worksheet(workbookSettings.workbook, "Analyze");

  worksheet_set_column_pixels(sheet, 0, 0, 200, NULL);
  worksheet_set_column_pixels(sheet, 1, 1, 200, NULL);

  int32_t rowOffset = 0;
  auto addTitle     = [&sheet, &rowOffset, &workbookSettings](const std::string &title) {
    rowOffset++;
    worksheet_merge_range(sheet, rowOffset, 0, rowOffset, 1, "-", workbookSettings.headerBold);
    worksheet_write_string(sheet, rowOffset, 0, title.data(), workbookSettings.headerBold);
    rowOffset++;
  };

  auto addElement = [&sheet, &rowOffset, &workbookSettings](const std::string &title, const std::string &value) {
    worksheet_write_string(sheet, rowOffset, 0, title.data(), workbookSettings.headerBold);
    worksheet_write_string(sheet, rowOffset, 1, value.data(), workbookSettings.fontNormal);
    rowOffset++;
  };

  addTitle("Date");
  addElement("Started at", joda::helper::timepointToIsoString(timeStarted));
  addElement("Finished at", joda::helper::timepointToIsoString(timeFinished));
  addElement("Duration at", joda::helper::getDurationAsString(timeStarted, timeFinished));

  addTitle("ImageC");
  addElement("Version", settings.imagecMeta.imagecVersion);
  addElement("Build", settings.imagecMeta.buildTime);

  addTitle("Classes");
  for(const auto &classs : settings.projectSettings.classification.classes) {
    nlohmann::json classIdStr = static_cast<enums::ClassId>(classs.classId);
    addElement(std::string(classIdStr), classs.name);
  }

  addTitle("Pipelines");
  for(const auto &pipeline : settings.pipelines) {
    addElement("Name", pipeline.meta.name);

    nlohmann::json classId = static_cast<enums::ClassId>(pipeline.pipelineSetup.defaultClassId);
    addElement("Default class", std::string(classId));
  }

  addTitle("Project settings");
  addElement("Scientist", settings.projectSettings.address.firstName + " " + settings.projectSettings.address.lastName);
  addElement("Organisation", settings.projectSettings.address.organization);
  addElement("Experiment ID", settings.projectSettings.experimentSettings.experimentId);
  addElement("Experiment name", settings.projectSettings.experimentSettings.experimentName);
  addElement("Job name", jobName);
  addElement("Notes", settings.projectSettings.experimentSettings.notes);
  addElement("Working directory", settings.projectSettings.workingDirectory);

  for(const auto &plate : settings.projectSettings.plates) {
    addTitle("Plate " + std::to_string(plate.plateId));
    addElement("Filename regex", plate.filenameRegex);
    addElement("Image folder", plate.imageFolder);
    addElement("Well order", joda::settings::vectorToString(plate.plateSetup.wellImageOrder));
    nlohmann::json groupBy = static_cast<enums::GroupBy>(plate.groupBy);
    addElement("Group by", std::string(groupBy));
  }
}

}    // namespace joda::db
