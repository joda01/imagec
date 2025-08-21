///
/// \file      exporter_xlsx.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <xlsxwriter/worksheet.h>
#include <string>
#include <variant>
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/table/table.hpp"
#include "controller/controller.hpp"
#include "exporter_xlsx.hpp"

namespace joda::exporter::xlsx {

void Exporter::startHeatmapExport(const std::vector<const Exportable *> &data, const settings::AnalyzeSettings &analyzeSettings,
                                  const std::string &jobName, std::chrono::system_clock::time_point timeStarted,
                                  std::chrono::system_clock::time_point timeFinished, const std::string &outputFileName,
                                  const settings::ResultsSettings &filterSettings, ExportSettings::ExportView view, int32_t imageHeight,
                                  int32_t imageWidth, const std::string &unit)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  auto workbookSettings = createWorkBook(outputFileName);

  createAnalyzeSettings(workbookSettings, analyzeSettings, jobName, timeStarted, timeFinished);

  int32_t cols           = filterSettings.getPlateSetup().cols;
  int32_t rows           = filterSettings.getPlateSetup().rows;
  int32_t densityMapSize = -1;
  switch(view) {
    case ExportSettings::ExportView::PLATE:
      break;
    case ExportSettings::ExportView::WELL:
      rows = filterSettings.getPlateSetup().getRowsAndColsOfWell().first;
      cols = filterSettings.getPlateSetup().getRowsAndColsOfWell().second;
      break;
    case ExportSettings::ExportView::IMAGE:
      densityMapSize = filterSettings.getDensityMapSettings().densityMapAreaSize;
      rows           = static_cast<int32_t>(std::ceil(static_cast<float>(imageHeight) / static_cast<float>(densityMapSize)));
      cols           = static_cast<int32_t>(std::ceil(static_cast<float>(imageWidth) / static_cast<float>(densityMapSize)));

      break;
  }

  auto colNr = filterSettings.getColumns().size();

  std::map<enums::ClassId, std::pair<std::string, std::vector<const table::TableColumn *>>> sortedTables;
  for(const auto &tbl : data) {
    for(uint32_t n = 0; n < colNr; n++) {
      auto &tmp = sortedTables[tbl->getTable().getColHeader(n).classId];
      tmp.first = tbl->getTable().columns().at(n).colSettings.names.className;
      tmp.second.emplace_back(&tbl->getTable().columns().at(n));
    }
  }

  int idx = 0;
  for(const auto &[clasId, columns] : sortedTables) {
    Pos pos;
    auto name = prepareSheetName(columns.first);
    name += "_" + std::to_string(idx);
    auto *worksheet = workbook_add_worksheet(workbookSettings.workbook, name.data());

    for(const auto &col : columns.second) {
      joda::table::Table tmpTable({*col});
      auto dataHeatmap = joda::db::data::convertToHeatmap(&tmpTable, rows, cols, 0, filterSettings.getFilter().tStack,
                                                          joda::db::data::PlotPlateSettings{.densityMapSize = densityMapSize});
      if(dataHeatmap.getNrOfRows() <= 0 || dataHeatmap.getNrOfCols() <= 0) {
        continue;
      }

      paintPlateBorder(worksheet, dataHeatmap.getNrOfRows(), dataHeatmap.getNrOfCols(), pos.row, workbookSettings.header,
                       workbookSettings.numberFormat, workbookSettings.merge_format, col->colSettings.createHeader(unit));
      pos = paintHeatmap(workbookSettings, worksheet, dataHeatmap, pos.row);
    }
    idx++;
  }
  workbook_close(workbookSettings.workbook);
}

///
/// \brief      Paint the borders of the heatmap
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Exporter::paintPlateBorder(lxw_worksheet *sheet, uint32_t rows, uint16_t cols, uint32_t rowOffset, lxw_format *header, lxw_format *numberFormat,
                                lxw_format *mergeFormat, const std::string &title)
{
  const int32_t ROW_OFFSET       = 2;
  const int32_t HEADER_CELL_SIZE = 15;
  const int32_t CELL_SIZE        = 60;

  ///////////////////////////7
  auto *condFormat          = new lxw_conditional_format();
  condFormat->type          = LXW_CONDITIONAL_3_COLOR_SCALE;
  condFormat->format        = numberFormat;
  condFormat->min_color     = 0x63BE7B;
  condFormat->min_rule_type = LXW_CONDITIONAL_RULE_TYPE_MINIMUM;
  condFormat->mid_color     = 0xFFEB84;
  condFormat->mid_rule_type = LXW_CONDITIONAL_RULE_TYPE_PERCENTILE;
  condFormat->mid_value     = 50;
  condFormat->max_color     = 0xF8696B;
  condFormat->max_rule_type = LXW_CONDITIONAL_RULE_TYPE_MAXIMUM;
  ///////////////////

  // Title
  worksheet_merge_range(sheet, rowOffset, 1, rowOffset, cols, "-", mergeFormat);
  worksheet_write_string(sheet, rowOffset, 1, title.data(), header);

  // Column
  worksheet_set_column_pixels(sheet, 0, 0, HEADER_CELL_SIZE, nullptr);
  worksheet_set_column_pixels(sheet, cols + 1, cols + 1, HEADER_CELL_SIZE, nullptr);

  for(uint16_t col = 1; col < cols + 1; col++) {
    worksheet_set_column_pixels(sheet, col, col, CELL_SIZE, nullptr);
    worksheet_write_string(sheet, rowOffset + 1, col, std::to_string(col).data(), header);
    worksheet_write_string(sheet, rows + rowOffset + 2, col, std::to_string(col).data(), header);
  }

  // Row
  worksheet_set_row_pixels(sheet, rowOffset + ROW_OFFSET, HEADER_CELL_SIZE, nullptr);
  for(uint32_t row = 0; row < rows; row++) {
    char toWrt[2];
    toWrt[0] = static_cast<char>((row) + 'A');
    toWrt[1] = 0;

    worksheet_set_row_pixels(sheet, row + rowOffset + ROW_OFFSET, CELL_SIZE, nullptr);
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
Exporter::Pos Exporter::paintHeatmap(const WorkBook &workbookSettings, lxw_worksheet *worksheet, const joda::table::Table &table, uint32_t rowOffset)
{
  const uint32_t ROW_OFFSET = 2;

  for(uint32_t row = 0; row < table.getNrOfRows(); row++) {
    for(uint16_t col = 0; col < table.getNrOfCols(); col++) {
      auto *format     = workbookSettings.numberFormatScientific;
      const auto &cell = table.data(row, col);

      if(!cell || !cell->isValid()) {
        format = workbookSettings.numberFormatInvalidScientific;
      }
      // Offset 2 because of title and plate numbering
      if(!cell || cell->isNAN()) {
        worksheet_write_blank(worksheet, ROW_OFFSET + row + rowOffset, 1 + col, format);
      } else {
        worksheet_write_number(worksheet, ROW_OFFSET + row + rowOffset, 1 + col, cell->getVal(), format);
      }
    }
  }

  return Exporter::Pos{.row = table.getNrOfRows() + rowOffset + ROW_OFFSET, .col = table.getNrOfCols()};
}

}    // namespace joda::exporter::xlsx
