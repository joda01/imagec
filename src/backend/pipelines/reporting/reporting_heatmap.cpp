
///
/// \file      reporting.cpp
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

#include "reporting_heatmap.hpp"
#include <xlsxwriter/worksheet.h>
#include <cstddef>
#include <exception>
#include <memory>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/roi/roi.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/logger/console_logger.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/settings/analze_settings_parser.hpp"
#include "reporting_helper.hpp"

namespace joda::pipeline::reporting {

void Heatmap::createHeatMapForImage(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                    const joda::results::ReportingContainer &containers, int64_t imageWidth,
                                    int64_t imageHeight, const std::string &fileName)
{
  lxw_workbook *workbook = workbook_new(fileName.data());
  // Well header
  lxw_format *header = workbook_add_format(workbook);
  format_set_bold(header);
  format_set_pattern(header, LXW_PATTERN_SOLID);
  format_set_bg_color(header, 0x002242);
  format_set_font_color(header, 0xFFFFFF);
  format_set_border(header, LXW_BORDER_THIN);
  format_set_font_size(header, 10);

  // Number format
  lxw_format *numberFormat = workbook_add_format(workbook);
  format_set_num_format(numberFormat, "0.00E+00");
  format_set_font_size(numberFormat, 10);
  format_set_align(numberFormat, LXW_ALIGN_CENTER);
  format_set_align(numberFormat, LXW_ALIGN_VERTICAL_CENTER);

  ////////////////////////////////////////////////////////////////////////////////////
  ////
  for(const auto heatMapSquareWidthIn :
      analyzeSettings.getReportingSettings().getHeatmapSettings().getImageHeatmapAreaWidth()) {
    struct Square
    {
      uint64_t nrOfValid  = 0;
      double avgIntensity = 0;
      double avgAreaSize  = 0;
      uint64_t cnt        = 0;
      uint64_t x          = 0;
      uint64_t y          = 0;
    };
    int64_t heatMapWidth = heatMapSquareWidthIn;
    if(heatMapWidth <= 0) {
      heatMapWidth = imageWidth;
    }
    int64_t nrOfSquaresX = (imageWidth / heatMapWidth) + 1;
    int64_t nrOfSquaresY = (imageHeight / heatMapWidth) + 1;

    auto *heatmapSquares = new std::vector<std::vector<Square>>(nrOfSquaresX);
    for(int64_t x = 0; x < nrOfSquaresX; x++) {
      heatmapSquares->at(x) = std::vector<Square>(nrOfSquaresY);
    }

    auto sheets = std::make_shared<std::map<int, lxw_worksheet *>>();

    //
    // Build the map
    //
    for(const auto &[channelIdx, table] : containers.mColumns) {
      std::string tabName = table.getTableName() + "_" + std::to_string(heatMapWidth) + "x" +
                            std::to_string(heatMapWidth) + "(" + std::to_string(channelIdx) + ")";
      if(!sheets->contains(channelIdx)) {
        sheets->emplace(channelIdx, workbook_add_worksheet(workbook, tabName.data()));
      }

      for(int row = 0; row < table.getNrOfRows(); row++) {
        if(table.columnKeyExists(static_cast<int>(Helper::ColumnIndexDetailedReport::CENTER_OF_MASS_X) | channelIdx) &&
           table.getTable()
               .at(table.getColIndexFromKey(static_cast<int>(Helper::ColumnIndexDetailedReport::CENTER_OF_MASS_X) |
                                            channelIdx))
               .contains(row)) {
          int64_t xCo = table.getTable()
                            .at(table.getColIndexFromKey(
                                static_cast<int>(Helper::ColumnIndexDetailedReport::CENTER_OF_MASS_X) | channelIdx))
                            .at(row)
                            .value;
          int64_t yCo = table.getTable()
                            .at(table.getColIndexFromKey(
                                static_cast<int>(Helper::ColumnIndexDetailedReport::CENTER_OF_MASS_Y) | channelIdx))
                            .at(row)
                            .value;
          if(xCo > imageWidth) {
            xCo = imageWidth;
          }
          if(yCo > imageHeight) {
            yCo = imageHeight;
          }

          int64_t squareXidx = xCo / heatMapWidth;
          int64_t squareYidx = yCo / heatMapWidth;

          double intensity = 0;
          double areaSize  = 0;
          bool valid       = false;
          if(table.getTable()
                 .at(table.getColIndexFromKey(static_cast<int>(Helper::ColumnIndexDetailedReport::INTENSITY_AVG) |
                                              channelIdx))
                 .contains(row)) {
            intensity = table.getTable()
                            .at(table.getColIndexFromKey(
                                static_cast<int>(Helper::ColumnIndexDetailedReport::INTENSITY_AVG) | channelIdx))
                            .at(row)
                            .value;
            valid = table.getTable()
                        .at(table.getColIndexFromKey(static_cast<int>(Helper::ColumnIndexDetailedReport::VALIDITY) |
                                                     channelIdx))
                        .at(row)
                        .validity == func::ParticleValidity::VALID;
          }
          if(table.getTable()
                 .at(table.getColIndexFromKey(static_cast<int>(Helper::ColumnIndexDetailedReport::AREA_SIZE) |
                                              channelIdx))
                 .contains(row)) {
            areaSize = table.getTable()
                           .at(table.getColIndexFromKey(static_cast<int>(Helper::ColumnIndexDetailedReport::AREA_SIZE) |
                                                        channelIdx))
                           .at(row)
                           .value;
          }

          if(valid) {
            heatmapSquares->at(squareXidx)[squareYidx].nrOfValid += 1;
            heatmapSquares->at(squareXidx)[squareYidx].avgIntensity += intensity;
            heatmapSquares->at(squareXidx)[squareYidx].avgAreaSize += areaSize;
            heatmapSquares->at(squareXidx)[squareYidx].cnt++;
          }

          if(heatmapSquares->at(squareXidx)[squareYidx].x == 0) {
            heatmapSquares->at(squareXidx)[squareYidx].x = xCo;
            heatmapSquares->at(squareXidx)[squareYidx].y = yCo;
          }
        }
      }

      ////////////////////////////////////////////////////////////////////////////////////
      //
      // Paint the heatmap
      const int ROW_OFFSET_START = 2;

      int rowOffset = ROW_OFFSET_START;
      worksheet_write_string(sheets->at(channelIdx), rowOffset - 1, 0, "Valid", NULL);
      paintPlateBorder(sheets->at(channelIdx), nrOfSquaresY, nrOfSquaresX, rowOffset, header, numberFormat);
      rowOffset = nrOfSquaresY + ROW_OFFSET_START + 4;
      worksheet_write_string(sheets->at(channelIdx), rowOffset - 1, 0, "Intensity", NULL);
      paintPlateBorder(sheets->at(channelIdx), nrOfSquaresY, nrOfSquaresX, rowOffset, header, numberFormat);
      rowOffset = 2 * nrOfSquaresY + ROW_OFFSET_START + ROW_OFFSET_START + 6;
      worksheet_write_string(sheets->at(channelIdx), rowOffset - 1, 0, "Area size", NULL);
      paintPlateBorder(sheets->at(channelIdx), nrOfSquaresY, nrOfSquaresX, rowOffset, header, numberFormat);

      for(int64_t x = 0; x < nrOfSquaresX; x++) {
        for(int64_t y = 0; y < nrOfSquaresY; y++) {
          rowOffset = ROW_OFFSET_START + 1;

          worksheet_write_number(sheets->at(channelIdx), rowOffset + y, x + 1,
                                 (double) heatmapSquares->at(x)[y].nrOfValid, numberFormat);
          rowOffset = nrOfSquaresY + ROW_OFFSET_START + 5;
          worksheet_write_number(sheets->at(channelIdx), rowOffset + y, x + 1,
                                 (double) heatmapSquares->at(x)[y].avgIntensity / (double) heatmapSquares->at(x)[y].cnt,
                                 numberFormat);
          rowOffset = 2 * nrOfSquaresY + ROW_OFFSET_START + ROW_OFFSET_START + 7;
          worksheet_write_number(sheets->at(channelIdx), rowOffset + y, x + 1,
                                 (double) heatmapSquares->at(x)[y].avgAreaSize / (double) heatmapSquares->at(x)[y].cnt,
                                 numberFormat);
          /*worksheet_write_number(sheets->at(channelIdx), rowOffset + y, x + 1,
                                 (double) heatmapSquares->at(x)[y].x * 1000000 + heatmapSquares->at(x)[y].y,
                                 numberFormat);*/
        }
      }
    }
    delete heatmapSquares;
  }
  workbook_close(workbook);
}

///
/// \brief      Create heatmap for all over reporting
/// \author     Joachim Danmayr
///
void Heatmap::createHeatmapOfWellsForGroup(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                           const std::string &outputFolder, const std::string &groupName,
                                           const std::string &jobName, const std::map<int32_t, HeatMapPoint> &wellOrder,
                                           int32_t sizeX, int32_t sizeY,
                                           const joda::results::ReportingContainer &groupReports)
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  std::string filename =
      outputFolder + separator + "heatmaps" + separator + "heatmap_" + groupName + "_" + jobName + ".xlsx";
  lxw_workbook *workbook   = nullptr;
  lxw_format *headerFormat = nullptr;
  lxw_format *numberFormat = nullptr;

  const int ROW_OFFSET_START = 2;
  const int COL_OFFSET       = 1;

  // Each column represents one channel. Each channel is printed to a separate worksheet
  for(const auto &[channelIdx, values] : groupReports.mColumns) {
    if(values.getTableName() == "INVALID" || groupName.empty() || groupName == "INVALID") {
      break;
    }
    if(nullptr == workbook) {
      workbook = workbook_new(filename.data());

      // Well header
      headerFormat = workbook_add_format(workbook);
      format_set_bold(headerFormat);
      format_set_pattern(headerFormat, LXW_PATTERN_SOLID);
      format_set_bg_color(headerFormat, 0x002242);
      format_set_font_color(headerFormat, 0xFFFFFF);
      format_set_border(headerFormat, LXW_BORDER_THIN);
      format_set_font_size(headerFormat, 10);

      // Number format
      numberFormat = workbook_add_format(workbook);
      format_set_num_format(numberFormat, "0.00E+00");
      format_set_font_size(numberFormat, 10);
      format_set_align(numberFormat, LXW_ALIGN_CENTER);
      format_set_align(numberFormat, LXW_ALIGN_VERTICAL_CENTER);
    }

    std::string wellName = groupName + "-" + values.getTableName();
    auto *worksheet      = workbook_add_worksheet(workbook, wellName.c_str());
    int nrOfRows         = sizeY;
    int nrOfCols         = sizeX;
    int rowOffset        = ROW_OFFSET_START;

    paintPlateBorder(worksheet, nrOfRows, nrOfCols, rowOffset, headerFormat, numberFormat);
    rowOffset++;
    for(int rowIdx = 0; rowIdx < values.getNrOfRows(); rowIdx++) {
      try {
        auto imageName = values.getRowNameAt(rowIdx);
        auto areaSize  = values.getTable()
                            .at(values.getColIndexFromKey(
                                static_cast<int>(Helper::ColumnIndexDetailedReport::VALIDITY) | channelIdx))
                            .at(rowIdx)
                            .value;
        auto imgNr =
            Helper::applyRegex(analyzeSettings.getReportingSettings().getHeatmapSettings().getFileRegex(), imageName)
                .img;
        auto pos = wellOrder.find(imgNr);
        if(pos != wellOrder.end()) {
          worksheet_write_number(worksheet, rowOffset + pos->second.y, pos->second.x + COL_OFFSET, (double) areaSize,
                                 numberFormat);
        }
      } catch(...) {
      }
    }
  }
  if(nullptr != workbook) {
    workbook_close(workbook);
  }
}

///
/// \brief      Create heatmap for all over reporting
/// \author     Joachim Danmayr
///
void Heatmap::createAllOverHeatMap(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                   std::map<std::string, joda::results::ReportingContainer> &allOverReport,
                                   const std::string &outputFolder, const std::string &fileName,
                                   const std::string &jobName,
                                   const std::vector<std::vector<int32_t>> &imageWellOrderMatrix)
{
  const int32_t PLATE_ROWS = 16;
  const int32_t PLATE_COLS = 24;

  const int32_t HEADER_CELL_SIZE = 15;

  const int32_t ROW_OFFSET_START = 2;

  lxw_workbook *workbook = workbook_new(fileName.data());
  // Well header
  lxw_format *header = workbook_add_format(workbook);
  format_set_bold(header);
  format_set_pattern(header, LXW_PATTERN_SOLID);
  format_set_bg_color(header, 0x002242);
  format_set_font_color(header, 0xFFFFFF);
  format_set_border(header, LXW_BORDER_THIN);
  format_set_font_size(header, 10);

  // Number format
  lxw_format *numberFormat = workbook_add_format(workbook);
  format_set_num_format(numberFormat, "0.00E+00");
  format_set_font_size(numberFormat, 10);
  format_set_align(numberFormat, LXW_ALIGN_CENTER);
  format_set_align(numberFormat, LXW_ALIGN_VERTICAL_CENTER);

  int32_t rowOffset = 0;

  // Intensity
  std::map<int, lxw_worksheet *> sheets;

  // Well matrix
  int32_t sizeX;
  int32_t sizeY;
  auto wellOrderMap            = transformMatrix(imageWellOrderMatrix, sizeX, sizeY);
  bool generateHeatmapForWells = !imageWellOrderMatrix.empty();

  // Draw heatmap
  // Each group should be one area in the heatmap whereby the groupname is GRC_<ROW-INDEX>_<COL-INDEX> in the heatmap
  for(const auto &[group, value] : allOverReport) {
    int row = -1;
    int col = -1;
    try {
      auto regexResult = Helper::applyGroupRegex(group);
      row              = regexResult.row;
      col              = regexResult.col;
    } catch(...) {
    }

    // If enabled we print for each group the heatmap of the wells of the group
    if(generateHeatmapForWells) {
      createHeatmapOfWellsForGroup(analyzeSettings, outputFolder, group, jobName, wellOrderMap, sizeX, sizeY, value);
    }

    // Each column represents one channel. Each channel is printed to a separate worksheet
    for(const auto &[channelIdx, values] : value.mColumns) {
      if(values.getTableName() == "INVALID") {
        break;
      }
      if(!sheets.contains(channelIdx)) {
        sheets[channelIdx] = workbook_add_worksheet(workbook, values.getTableName().data());
        rowOffset          = ROW_OFFSET_START;
        worksheet_merge_range(sheets.at(channelIdx), rowOffset - 1, 0, rowOffset - 1, PLATE_COLS + 1, "-", NULL);
        worksheet_write_string(sheets.at(channelIdx), rowOffset - 1, 0, "Valid", NULL);
        paintPlateBorder(sheets.at(channelIdx), PLATE_ROWS, PLATE_COLS, rowOffset, header, numberFormat);
        rowOffset = PLATE_ROWS + ROW_OFFSET_START + 4;
        worksheet_merge_range(sheets.at(channelIdx), rowOffset - 1, 0, rowOffset - 1, PLATE_COLS + 1, "-", NULL);
        worksheet_write_string(sheets.at(channelIdx), rowOffset - 1, 0, "Intensity", NULL);
        paintPlateBorder(sheets.at(channelIdx), PLATE_ROWS, PLATE_COLS, rowOffset, header, numberFormat);
        rowOffset = 2 * PLATE_ROWS + ROW_OFFSET_START + ROW_OFFSET_START + 6;
        worksheet_merge_range(sheets.at(channelIdx), rowOffset - 1, 0, rowOffset - 1, PLATE_COLS + 1, "-", NULL);
        worksheet_write_string(sheets.at(channelIdx), rowOffset - 1, 0, "Area size", NULL);
        paintPlateBorder(sheets.at(channelIdx), PLATE_ROWS, PLATE_COLS, rowOffset, header, numberFormat);
      }

      auto *sheet = sheets.at(channelIdx);
      try {
        if(row >= 0 && col >= 0) {
          rowOffset = ROW_OFFSET_START;

          if(generateHeatmapForWells) {
            std::string filePath = "external:.\\heatmaps/heatmap_" + group + "_" + jobName + ".xlsx";
            worksheet_write_url(sheet, rowOffset + row, col, filePath.data(), NULL);
          }

          worksheet_write_number(sheet, rowOffset + row, col,
                                 values.getStatistics()
                                     .at(values.getColIndexFromKey(
                                         static_cast<int>(Helper::ColumnIndexDetailedReport::VALIDITY) | channelIdx))
                                     .getAvg(),
                                 numberFormat);

          rowOffset = PLATE_ROWS + ROW_OFFSET_START + 4;

          if(generateHeatmapForWells) {
            std::string filePath = "external:.\\heatmaps/heatmap_" + group + "_" + jobName + ".xlsx";
            worksheet_write_url(sheet, rowOffset + row, col, filePath.data(), NULL);
          }
          worksheet_write_number(
              sheet, rowOffset + row, col,
              values.getStatistics()
                  .at(values.getColIndexFromKey(static_cast<int>(Helper::ColumnIndexDetailedReport::INTENSITY_AVG) |
                                                channelIdx))
                  .getAvg(),
              numberFormat);

          rowOffset = 2 * PLATE_ROWS + ROW_OFFSET_START + ROW_OFFSET_START + 6;

          if(generateHeatmapForWells) {
            std::string filePath = "external:.\\heatmaps/heatmap_" + group + "_" + jobName + ".xlsx";
            worksheet_write_url(sheet, rowOffset + row, col, filePath.data(), NULL);
          }
          worksheet_write_number(sheet, rowOffset + row, col,
                                 values.getStatistics()
                                     .at(values.getColIndexFromKey(
                                         static_cast<int>(Helper::ColumnIndexDetailedReport::AREA_SIZE) | channelIdx))
                                     .getAvg(),
                                 numberFormat);
        }
      } catch(...) {
        // No data
      }
    }
  }

  workbook_close(workbook);
}

///
/// \brief      Paint the borders of the heatmap
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset, lxw_format *header,
                               lxw_format *numberFormat)
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

///
/// \brief      Transforms a 2D Matrix where the elements in the matrix represents an image index
///             and the coordinates of the matrix the position on the well to a map
///             whereby the key is the image index and the values are the coordinates
///              | 0  1  2
///             -|---------
///             0| 1  2  3
///             1| 4  5  6
///             2| 7  8  9
///
///            [1] => {0,0}
///            [2] => {1,0}
///            ...
///            [9] => {2,2}
///
///
/// \author     Joachim Danmayr
///
auto Heatmap::transformMatrix(const std::vector<std::vector<int32_t>> &imageWellOrderMatrix, int32_t &sizeX,
                              int32_t &sizeY) -> std::map<int32_t, HeatMapPoint>
{
  sizeY = imageWellOrderMatrix.size();
  sizeX = 0;

  std::map<int32_t, HeatMapPoint> ret;
  for(int y = 0; y < imageWellOrderMatrix.size(); y++) {
    for(int x = 0; x < imageWellOrderMatrix[y].size(); x++) {
      ret[imageWellOrderMatrix[y][x]] = HeatMapPoint{.x = x, .y = y};
      if(x > sizeX) {
        sizeX = x;
      }
    }
  }
  sizeX++;    // Because we start with zro to count
  return ret;
}

}    // namespace joda::pipeline::reporting