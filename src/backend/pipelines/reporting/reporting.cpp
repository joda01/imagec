
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

#include "reporting.hpp"
#include <xlsxwriter/worksheet.h>
#include <cstddef>
#include <exception>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <string>
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/roi/roi.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/logger/console_logger.hpp"
#include "backend/pipelines/processor/image_processor.hpp"

namespace joda::pipeline {

Reporting::Reporting(const joda::settings::json::AnalyzeSettings &settings) : mAnalyzeSettings(settings)
{
}

///
/// \brief      Set detail report header
/// \author     Joachim Danmayr
///
void Reporting::setDetailReportHeader(joda::reporting::ReportingContainer &detailReportTable,
                                      const std::string &channelName, int tempChannelIdx)
{
  try {
    detailReportTable.getTableAt(tempChannelIdx, channelName)
        .setColumnNames({{static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE), "confidence"},
                         {static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE), "areaSize"},
                         {static_cast<int>(ColumnIndexDetailedReport::PERIMETER), "perimeter"},
                         {static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY), "circularity"},
                         {static_cast<int>(ColumnIndexDetailedReport::VALIDITY), "validity"},
                         {static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_X), "x"},
                         {static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_Y), "y"},
                         {static_cast<int>(ColumnIndexDetailedReport::INTENSITY), "intensity"},
                         {static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN), "intensity min"},
                         {static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX), "intensity max"}});
  } catch(const std::exception &ex) {
    std::cout << "Pipeline::setDetailReportHeader >" << ex.what() << "<" << std::endl;
  }
}

///
/// \brief      Append to detailed report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
///
void Reporting::appendToDetailReport(joda::func::DetectionResponse &result,
                                     joda::reporting::ReportingContainer &detailReportTable,
                                     const std::string &detailReportOutputPath, int realChannelIdx, int tempChannelIdx,
                                     uint32_t tileIdx, const ImageProperties &imgProps)
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  // Free memory
  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(0);

  auto id = DurationCount::start("write-control-image");
  if(!result.controlImage.empty()) {
    cv::imwrite(detailReportOutputPath + separator + "control_" + std::to_string(tempChannelIdx) + "_" +
                    std::to_string(tileIdx) + ".png",
                result.controlImage, compression_params);
  } else {
    std::cout << "CTRL img null" << std::endl;
  }
  // if(!result.originalImage.empty()) {
  //   cv::imwrite(detailReportOutputPath + separator + "original_" + std::to_string(tempChannelIdx) + "_" +
  //                   std::to_string(tileIdx) + ".png",
  //               result.originalImage * ((float) UINT8_MAX / (float) UINT16_MAX), compression_params);
  // }

  auto [offsetX, offsetY] =
      TiffLoader::calculateTileXYoffset(joda::algo::TILES_TO_LOAD_PER_RUN, tileIdx, imgProps.width, imgProps.height,
                                        imgProps.tileWidth, imgProps.tileHeight);

  int64_t xMul = offsetX * imgProps.tileWidth;
  int64_t yMul = offsetY * imgProps.tileHeight;

  std::cout << "XOFF: " << std::to_string(xMul) << " | "
            << "YOFF: " << std::to_string(yMul) << std::endl;

  std::cout << "w: " << std::to_string(imgProps.width) << " | "
            << "h: " << std::to_string(imgProps.height) << std::endl;

  std::cout << "tw: " << std::to_string(imgProps.tileWidth) << " | "
            << "th: " << std::to_string(imgProps.tileHeight) << std::endl;

  std::cout << "xo: " << std::to_string(offsetX) << " | "
            << "yo: " << std::to_string(offsetY) << std::endl;

  std::cout << "tileidx: " << std::to_string(tileIdx) << std::endl;

  DurationCount::stop(id);
  int64_t indexOffset = 0;
  {
    std::lock_guard<std::mutex> lock(mAppendMutex);
    indexOffset = detailReportTable.getTableAt(tempChannelIdx, "")
                      .getNrOfRowsAtColumn(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
  }
  for(const auto &imgData : result.result) {
    try {
      int64_t index = imgData.getIndex() + indexOffset;

      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE), index,
                                    imgData.getConfidence(), imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE), index,
                                    imgData.getAreaSize(), imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::PERIMETER), index,
                                    imgData.getPerimeter(), imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY), index,
                                    imgData.getCircularity(), imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::VALIDITY), index,
                                    imgData.getValidity());

      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_X), index,
                                    imgData.getCenterOfMass().x + xMul, imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_Y), index,
                                    imgData.getCenterOfMass().y + yMul, imgData.getValidity());

      int idxOffset = 0;
      for(const auto &[channelIndexIn, intensity] : imgData.getIntensity()) {
        int channelIndex = channelIndexIn;
        if(channelIndex < 0) {
          channelIndex = realChannelIdx;
        }
        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY) + idxOffset, index,
                                      intensity.intensity, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::INTENSITY) + idxOffset,
                           "#intensity avg " + mAnalyzeSettings.getChannelNameOfIndex(channelIndex));

        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN) + idxOffset, index,
                                      intensity.intensityMax, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN) + idxOffset,
                           "#intensity min " + mAnalyzeSettings.getChannelNameOfIndex(channelIndex));

        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX) + idxOffset, index,
                                      intensity.intensityMin, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX) + idxOffset,
                           "#intensity max " + mAnalyzeSettings.getChannelNameOfIndex(channelIndex));
        idxOffset += 3;    // intnsity avg, min and max are 3 columns
      }
    } catch(const std::exception &ex) {
      std::string msg = "Pipeline::appendToDetailReport >" + std::string(ex.what()) + "<";
      joda::log::logWarning(msg);
    }
  }
}

///
/// \brief      Append to all over report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
/// \param[in]  nrOfChannels Nr. of channels
///
void Reporting::appendToAllOverReport(std::map<std::string, joda::reporting::ReportingContainer> &allOverReport,
                                      const joda::reporting::ReportingContainer &detailedReport,
                                      const std::string &imagePath, const std::string &imageName, int nrOfChannels)
{
  const int NR_OF_COLUMNS_PER_CHANNEL = 7;
  try {
    for(int tempChannelIdx = 0; tempChannelIdx < nrOfChannels; tempChannelIdx++) {
      allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
          .getTableAt(tempChannelIdx, detailedReport.getTableAt(tempChannelIdx).getTableName())
          .setColumnNames({
              {0, "#valid"},
              {1, "#invalid"},
              {2, detailedReport.getTableAt(tempChannelIdx)
                      .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE))},
              {3, detailedReport.getTableAt(tempChannelIdx)
                      .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE))},
              {4, detailedReport.getTableAt(tempChannelIdx)
                      .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::PERIMETER))},
              {5, detailedReport.getTableAt(tempChannelIdx)
                      .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY))},
              {6, detailedReport.getTableAt(tempChannelIdx)
                      .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::INTENSITY))},
          });

      /// \todo Copy constructor ia called here
      auto colStatistics = detailedReport.getTableAt(tempChannelIdx)
                               .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
      allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(0, colStatistics.getNr(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
      allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(1, colStatistics.getInvalid(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
      allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(2, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE));
      allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(3, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::PERIMETER));
      allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(4, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY));
      allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(5, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

      //
      int64_t column = 6;
      int stasOffset = 0;
      int rowIdx     = 0;
      while(detailedReport.getTableAt(tempChannelIdx)
                .containsStatistics(static_cast<int>(ColumnIndexDetailedReport::INTENSITY) + stasOffset)) {
        colStatistics = detailedReport.getTableAt(tempChannelIdx)
                            .getStatistics(static_cast<int>(ColumnIndexDetailedReport::INTENSITY) + stasOffset);

        rowIdx = allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
                     .getTableAt(tempChannelIdx, "")
                     .appendValueToColumn(column, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

        allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
            .getTableAt(tempChannelIdx, "")
            .setColumnName(column,
                           detailedReport.getTableAt(tempChannelIdx)
                               .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::INTENSITY) + stasOffset));

        stasOffset += 3;    // intensity avg, min, max
        column++;
      }
      // This tells the table how many rows are available
      allOverReport[getGroupToStoreImageIn(imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .setRowName(rowIdx, imageName);
    }
  } catch(const std::exception &ex) {
    std::cout << "Pipeline::appendToAllOverReport >" << ex.what() << "<" << std::endl;
  }
}

void Reporting::createHeatMapForImage(const joda::reporting::ReportingContainer &containers, int64_t imageWidth,
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
  for(const auto heatMapSquareWidth : mAnalyzeSettings.getReportingSettings().getImageHeatmapAreaWidth()) {
    struct Square
    {
      uint64_t nrOfValid  = 0;
      double avgIntensity = 0;
      double avgAreaSize  = 0;
      uint64_t cnt        = 0;
    };

    int64_t nrOfSquaresX = (imageWidth / heatMapSquareWidth) + 1;
    int64_t nrOfSquaresY = (imageHeight / heatMapSquareWidth) + 1;

    auto *heatmapSquares = new std::vector<std::vector<Square>>(nrOfSquaresX);
    for(int64_t x = 0; x < nrOfSquaresX; x++) {
      heatmapSquares->at(x) = std::vector<Square>(nrOfSquaresY);
    }
    std::map<int, lxw_worksheet *> sheets;

    //
    // Build the map
    //
    for(const auto &[channelIdx, table] : containers.mColumns) {
      std::string tabName =
          table.getTableName() + "_" + std::to_string(heatMapSquareWidth) + "x" + std::to_string(heatMapSquareWidth);
      sheets[channelIdx] = workbook_add_worksheet(workbook, tabName.data());

      for(int row = 0; row < table.getNrOfRows(); row++) {
        if(table.getTable().at(static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_X)).contains(row)) {
          int64_t xCo =
              table.getTable().at(static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_X)).at(row).value;
          int64_t yCo =
              table.getTable().at(static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_Y)).at(row).value;
          if(xCo > imageWidth) {
            xCo = imageWidth;
          }
          if(yCo > imageHeight) {
            yCo = imageHeight;
          }

          int64_t squareXidx = xCo / heatMapSquareWidth;
          int64_t squareYidx = yCo / heatMapSquareWidth;

          double intensity = 0;
          double areaSize  = 0;
          bool valid       = false;
          if(table.getTable().at(static_cast<int>(ColumnIndexDetailedReport::INTENSITY)).contains(row)) {
            intensity = table.getTable().at(static_cast<int>(ColumnIndexDetailedReport::INTENSITY)).at(row).value;
            valid     = table.getTable().at(static_cast<int>(ColumnIndexDetailedReport::VALIDITY)).at(row).validity ==
                    func::ParticleValidity::VALID;
          }
          if(table.getTable().at(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE)).contains(row)) {
            areaSize = table.getTable().at(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE)).at(row).value;
          }

          if(valid) {
            heatmapSquares->at(squareXidx)[squareYidx].nrOfValid += 1;
            heatmapSquares->at(squareXidx)[squareYidx].avgIntensity += intensity;
            heatmapSquares->at(squareXidx)[squareYidx].avgAreaSize += areaSize;
            heatmapSquares->at(squareXidx)[squareYidx].cnt++;
          }
        }
      }

      ////////////////////////////////////////////////////////////////////////////////////
      //
      // Paint the heatmap
      const int ROW_OFFSET_START = 2;

      int rowOffset = ROW_OFFSET_START;
      worksheet_write_string(sheets.at(channelIdx), rowOffset - 1, 0, "Valid", NULL);
      paintPlateBorder(sheets.at(channelIdx), nrOfSquaresY, nrOfSquaresX, rowOffset, header, numberFormat);
      rowOffset = nrOfSquaresY + ROW_OFFSET_START + 4;
      worksheet_write_string(sheets.at(channelIdx), rowOffset - 1, 0, "Intensity", NULL);
      paintPlateBorder(sheets.at(channelIdx), nrOfSquaresY, nrOfSquaresX, rowOffset, header, numberFormat);
      rowOffset = 2 * nrOfSquaresY + ROW_OFFSET_START + ROW_OFFSET_START + 6;
      worksheet_write_string(sheets.at(channelIdx), rowOffset - 1, 0, "Area size", NULL);
      paintPlateBorder(sheets.at(channelIdx), nrOfSquaresY, nrOfSquaresX, rowOffset, header, numberFormat);

      for(int64_t x = 0; x < nrOfSquaresX; x++) {
        for(int64_t y = 0; y < nrOfSquaresY; y++) {
          rowOffset = ROW_OFFSET_START + 1;

          worksheet_write_number(sheets.at(channelIdx), rowOffset + y, x + 1,
                                 (double) heatmapSquares->at(x)[y].nrOfValid, numberFormat);
          rowOffset = nrOfSquaresY + ROW_OFFSET_START + 5;
          worksheet_write_number(sheets.at(channelIdx), rowOffset + y, x + 1,
                                 (double) heatmapSquares->at(x)[y].avgIntensity / (double) heatmapSquares->at(x)[y].cnt,
                                 numberFormat);
          rowOffset = 2 * nrOfSquaresY + ROW_OFFSET_START + ROW_OFFSET_START + 7;
          worksheet_write_number(sheets.at(channelIdx), rowOffset + y, x + 1,
                                 (double) heatmapSquares->at(x)[y].avgAreaSize / (double) heatmapSquares->at(x)[y].cnt,
                                 numberFormat);
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
void Reporting::createAllOverHeatMap(std::map<std::string, joda::reporting::ReportingContainer> &allOverReport,
                                     const std::string &fileName)
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

  // Draw heatmap
  for(const auto &[group, value] : allOverReport) {
    for(const auto &[channelIdx, values] : value.mColumns) {
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
        auto row = applyRegex(mAnalyzeSettings.getReportingSettings().getFileRegex(), group).row;
        auto col = applyRegex(mAnalyzeSettings.getReportingSettings().getFileRegex(), group).col;

        rowOffset = ROW_OFFSET_START;
        worksheet_write_number(
            sheet, rowOffset + row, col,
            values.getStatistics().at(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE)).getAvg(), numberFormat);

        rowOffset = PLATE_ROWS + ROW_OFFSET_START + 4;
        worksheet_write_number(
            sheet, rowOffset + row, col,
            values.getStatistics().at(static_cast<int>(ColumnIndexDetailedReport::INTENSITY)).getAvg(), numberFormat);

        rowOffset = 2 * PLATE_ROWS + ROW_OFFSET_START + ROW_OFFSET_START + 6;
        worksheet_write_number(
            sheet, rowOffset + row, col,
            values.getStatistics().at(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE)).getAvg(), numberFormat);
      } catch(const std::exception &) {
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
void Reporting::paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset,
                                 lxw_format *header, lxw_format *numberFormat) const
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
/// \brief      Depending on the settings images could be stored in folder, filename or no group
/// \author     Joachim Danmayr
///
auto Reporting::getGroupToStoreImageIn(const std::string &imagePath, const std::string &imageName) -> std::string
{
  switch(mAnalyzeSettings.getReportingSettings().getGroupBy()) {
    case settings::json::AnalyzeSettingsReporting::GroupBy::OFF:
      return {};
    case settings::json::AnalyzeSettingsReporting::GroupBy::FOLDER:
      return imagePath;
    case settings::json::AnalyzeSettingsReporting::GroupBy::FILENAME:
      try {
        return applyRegex(mAnalyzeSettings.getReportingSettings().getFileRegex(), imageName).group;
      } catch(const std::exception &) {
        return "invalid_name";
      }
  }
  return "invalid_name";
}

///
/// \brief      Apply regex
/// \author     Joachim Danmayr
///
Reporting::RegexResult Reporting::applyRegex(const std::string &regex, const std::string &fileName)
{
  auto stringToNumber = [](const std::string &str) {
    int result = 0;
    for(char c : str) {
      if(isdigit(c)) {
        result = result * 10 + (c - '0');    // Convert digit character to integer
      } else if(isalpha(c)) {
        result = result * 10 + (toupper(c) - 'A' + 1);    // Convert alphabetic character to integer
      } else {
        std::cerr << "Invalid character encountered: " << c << std::endl;
      }
    }
    return result;
  };

  std::regex pattern(regex);
  std::smatch match;
  Reporting::RegexResult result;

  if(std::regex_search(fileName, match, pattern)) {
    if(match.size() >= 4) {
      result.group = match[0].str();
      result.row   = stringToNumber(match[2].str());
      result.col   = stringToNumber(match[3].str());
    } else {
      throw std::invalid_argument("Pattern not found.");
    }
  } else {
    throw std::invalid_argument("Pattern not found.");
  }
  return result;
}

}    // namespace joda::pipeline
