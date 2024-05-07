
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
#include <xlsxwriter/workbook.h>
#include <xlsxwriter/worksheet.h>
#include <cstddef>
#include <exception>
#include <memory>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/roi/roi.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/logger/console_logger.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/results/results.hpp"
#include "backend/results/results_defines.hpp"
#include "backend/results/results_helper.hpp"
#include "backend/results/results_image_meta.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/settings.hpp"

namespace joda::pipeline::reporting {

void Heatmap::createHeatMapForImage(const std::set<int32_t> &imageHeatmapAreaSizes,
                                    const joda::results::ReportingSettings &reportingSettings,
                                    const joda::results::WorkSheet &containers, const std::string &fileName)
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

  // Number format invalid
  lxw_format *numberFormatInvalid = workbook_add_format(workbook);
  numberFormatInvalid             = workbook_add_format(workbook);
  format_set_num_format(numberFormatInvalid, "0.00E+00");
  format_set_font_size(numberFormatInvalid, 10);
  format_set_align(numberFormatInvalid, LXW_ALIGN_CENTER);
  format_set_align(numberFormatInvalid, LXW_ALIGN_VERTICAL_CENTER);
  // format_set_border(numberFormatInvalid, LXW_BORDER_THIN);                 // Set border style to thin
  format_set_diag_type(numberFormatInvalid, LXW_DIAGONAL_BORDER_UP_DOWN);

  ////////////////////////////////////////////////////////////////////////////////////
  ////
  auto imageMeta = containers.getImageMeta();
  if(!imageMeta.has_value()) {
    throw std::runtime_error("No image meta!");
  }

  for(const auto heatMapSquareWidthIn : imageHeatmapAreaSizes) {
    //
    // Build the map
    //
    for(const auto &[groupName, group] : containers.getGroups()) {
      for(const auto &[channelIdx, results] : group.getChannels()) {
        struct Square
        {
          std::map<MeasureKey, double> vals;
          uint64_t nrOfValid = 0;
          uint64_t cnt       = 0;
          uint64_t x         = 0;
          uint64_t y         = 0;
        };
        int64_t heatMapWidth = heatMapSquareWidthIn;
        if(heatMapWidth <= 0) {
          heatMapWidth = imageMeta->width;
        }
        int64_t nrOfSquaresX = (imageMeta->width / heatMapWidth) + 1;
        int64_t nrOfSquaresY = (imageMeta->height / heatMapWidth) + 1;

        auto *heatmapSquares = new std::vector<std::vector<Square>>(nrOfSquaresX);
        for(int64_t x = 0; x < nrOfSquaresX; x++) {
          heatmapSquares->at(x) = std::vector<Square>(nrOfSquaresY);
        }

        std::string tabName = results.getChannelMeta().name + "_" + std::to_string(heatMapWidth) + "x" +
                              std::to_string(heatMapWidth) + "(" + joda::settings::to_string(channelIdx) + ")";
        auto *worksheet = workbook_add_worksheet(workbook, tabName.data());

        const int ROW_OFFSET_START = 2;
        int rowOffset              = ROW_OFFSET_START;
        for(auto const &[measureCh, measureChMeta] : results.getMeasuredChannels()) {
          if(reportingSettings.detail.measureChannels.contains(measureCh.getMeasureChannel())) {
            // Create Frames

            // Paint the heatmap header
            worksheet_write_string(worksheet, rowOffset - 1, 0, measureChMeta.name.data(), NULL);
            paintPlateBorder(worksheet, nrOfSquaresY, nrOfSquaresX, rowOffset, header, numberFormat);
            rowOffset += nrOfSquaresY + ROW_OFFSET_START + 4;

            // Init channel values with 0
            for(int64_t x = 0; x < nrOfSquaresX; x++) {
              for(int64_t y = 0; y < nrOfSquaresY; y++) {
                if(!heatmapSquares->at(x)[y].vals.contains(measureCh)) {
                  heatmapSquares->at(x)[y].vals[measureCh] = 0;
                }
              }
            }
          }
        }

        for(const auto &[objKey, image] : results.getObjects()) {
          auto measureKeyX =
              joda::results::MeasureChannelKey{results::ReportingSettings::MeasureChannels::CENTER_OF_MASS_X,
                                               results::ReportingSettings::MeasureChannelStat::VAL, channelIdx};
          auto measureKeyY =
              joda::results::MeasureChannelKey{results::ReportingSettings::MeasureChannels::CENTER_OF_MASS_Y,
                                               results::ReportingSettings::MeasureChannelStat::VAL, channelIdx};
          auto xCo = std::get<double>(image.getMeasurements().at(measureKeyX).getVal());
          auto yCo = std::get<double>(image.getMeasurements().at(measureKeyY).getVal());

          if(xCo > imageMeta->width) {
            xCo = imageMeta->width;
          }
          if(yCo > imageMeta->height) {
            yCo = imageMeta->height;
          }

          int64_t squareXidx = xCo / heatMapWidth;
          int64_t squareYidx = yCo / heatMapWidth;

          for(const auto &[measKey, val] : image.getMeasurements()) {
            if(reportingSettings.detail.measureChannels.contains(measKey.getMeasureChannel())) {
              // Fill out with value
              if(image.getObjectMeta().valid) {
                heatmapSquares->at(squareXidx)[squareYidx].nrOfValid += 1;
                heatmapSquares->at(squareXidx)[squareYidx].vals[measKey] += std::get<double>(val.getVal());
                heatmapSquares->at(squareXidx)[squareYidx].cnt++;
              }

              if(heatmapSquares->at(squareXidx)[squareYidx].x == 0) {
                heatmapSquares->at(squareXidx)[squareYidx].x = xCo;
                heatmapSquares->at(squareXidx)[squareYidx].y = yCo;
              }
            }
          }
        }
        rowOffset = ROW_OFFSET_START;

        ////////////////////////////////////////////////////////////////////////////////////
        //
        // Paint the heatmap

        for(int64_t x = 0; x < nrOfSquaresX; x++) {
          for(int64_t y = 0; y < nrOfSquaresY; y++) {
            rowOffset = ROW_OFFSET_START + 1;
            for(const auto &[_, val] : heatmapSquares->at(x)[y].vals) {
              double cnt = heatmapSquares->at(x)[y].cnt;
              if(cnt > 0) {
                worksheet_write_number(worksheet, rowOffset + y, x + 1, val / cnt, numberFormat);
              } else {
                worksheet_write_number(worksheet, rowOffset + y, x + 1, 0, numberFormatInvalid);
              }
              rowOffset += nrOfSquaresY + ROW_OFFSET_START + 4;
            }
          }
        }
        delete heatmapSquares;
      }
    }
  }

  workbook_close(workbook);
}

///
/// \brief      Create heatmap for all over reporting
/// \author     Joachim Danmayr
///
void Heatmap::createHeatmapOfWellsForGroup(const joda::results::ReportingSettings &reportingSettings,
                                           const std::string &outputFileName, const std::string &groupName,
                                           const std::map<int32_t, results::ImgPositionInWell> &wellOrder,
                                           int32_t wellSizeX, int32_t wellSizeY,
                                           const joda::results::Group &groupReports)
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  lxw_workbook *workbook          = nullptr;
  lxw_format *headerFormat        = nullptr;
  lxw_format *numberFormat        = nullptr;
  lxw_format *numberFormatInvalid = nullptr;

  const int ROW_OFFSET_START = 2;
  const int COL_OFFSET       = 1;

  // Each column represents one channel. Each channel is printed to a separate worksheet
  for(const auto &[channelIdx, results] : groupReports.getChannels()) {
    if(results.getChannelMeta().name == "INVALID" || groupName.empty() || groupName == "INVALID") {
      break;
    }
    if(nullptr == workbook) {
      workbook = workbook_new(outputFileName.data());

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

      // Number format invalid
      numberFormatInvalid = workbook_add_format(workbook);
      format_set_num_format(numberFormatInvalid, "0.00E+00");
      format_set_font_size(numberFormatInvalid, 10);
      format_set_align(numberFormatInvalid, LXW_ALIGN_CENTER);
      format_set_align(numberFormatInvalid, LXW_ALIGN_VERTICAL_CENTER);
      // format_set_border(numberFormatInvalid, LXW_BORDER_THIN);                 // Set border style to thin
      format_set_diag_type(numberFormatInvalid, LXW_DIAGONAL_BORDER_UP_DOWN);    // Set diagonal type
    }

    std::string wellName = groupName + "-" + results.getChannelMeta().name;
    auto *worksheet      = workbook_add_worksheet(workbook, wellName.c_str());
    int nrOfRows         = wellSizeY;
    int nrOfCols         = wellSizeX;
    int rowOffset        = ROW_OFFSET_START;

    for(auto const &[measureCh, measureChMeta] : results.getMeasuredChannels()) {
      if(reportingSettings.detail.measureChannels.contains(measureCh.getMeasureChannel())) {
        auto writePlateFrame = [&worksheet, &rowOffset, &nrOfRows, &nrOfCols, &numberFormat, &numberFormatInvalid,
                                &headerFormat](const std::string &value) {
          worksheet_merge_range(worksheet, rowOffset - 1, 0, rowOffset - 1, nrOfCols + 1, "-", NULL);
          worksheet_write_string(worksheet, rowOffset - 1, 0, value.data(), NULL);
          paintPlateBorder(worksheet, nrOfRows, nrOfCols, rowOffset, headerFormat, numberFormat);
        };

        writePlateFrame(measureChMeta.name);
        rowOffset = rowOffset + nrOfRows + ROW_OFFSET_START + 4;
      }
    }

    for(const auto &[objKey, image] : results.getObjects()) {
      rowOffset = ROW_OFFSET_START + 1;
      for(const auto &[measKey, val] : image.getMeasurements()) {
        if(reportingSettings.detail.measureChannels.contains(measKey.getMeasureChannel())) {
          if(std::holds_alternative<double>(val.getVal())) {
            auto imgMeta = image.getImageMeta();
            if(!imgMeta.has_value()) {
              throw std::runtime_error("There is an image detail report without image meta!");
            }
            double value = std::get<double>(val.getVal());
            auto *format = numberFormat;
            if(!image.getObjectMeta().valid) {
              format = numberFormatInvalid;
            }
            worksheet_write_number(worksheet, rowOffset + imgMeta->imgPosInWell.y, imgMeta->imgPosInWell.x + COL_OFFSET,
                                   value, format);

            rowOffset = rowOffset + nrOfRows + ROW_OFFSET_START + 4;
          }
        }
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
void Heatmap::createAllOverHeatMap(const joda::results::ReportingSettings &reportingSettings,
                                   joda::results::WorkSheet &allOverReport, const std::string &outputFolder)
{
  const int32_t PLATE_ROWS = 16;
  const int32_t PLATE_COLS = 24;

  const int32_t HEADER_CELL_SIZE = 15;

  const int32_t ROW_OFFSET_START = 2;

  lxw_workbook *workbook = workbook_new(outputFolder.data());
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

  // Number format invalid
  lxw_format *numberFormatInvalid = workbook_add_format(workbook);
  format_set_num_format(numberFormatInvalid, "0.00E+00");
  format_set_font_size(numberFormatInvalid, 10);
  format_set_align(numberFormatInvalid, LXW_ALIGN_CENTER);
  format_set_align(numberFormatInvalid, LXW_ALIGN_VERTICAL_CENTER);
  // format_set_border(numberFormatInvalid, LXW_BORDER_THIN);                 // Set border style to thin
  format_set_diag_type(numberFormatInvalid, LXW_DIAGONAL_BORDER_UP_DOWN);    // Set diagonal type

  int32_t rowOffset = 0;

  // Intensity
  std::map<joda::settings::ChannelIndex, lxw_worksheet *> sheets;

  std::optional<results::ExperimentMeta> expMeta = allOverReport.getExperimentMeta();
  if(!expMeta.has_value()) {
    throw std::runtime_error("There is a report without experiment meta data!");
  }

  // Well matrix
  int32_t sizeX;
  int32_t sizeY;
  auto wellOrderMap            = expMeta->transformMatrix(sizeX, sizeY);
  bool generateHeatmapForWells = !expMeta->wellImageOrder.empty();

  // Draw heatmap
  // Each group should be one area in the heatmap whereby the groupname is GRC_<ROW-INDEX>_<COL-INDEX> in the heatmap
  for(const auto &[group, value] : allOverReport.getGroups()) {
    int row = -1;
    int col = -1;
    try {
      auto regexResult = joda::results::Helper::applyGroupRegex(group);
      row              = regexResult.row;
      col              = regexResult.col;
    } catch(...) {
    }

    // If enabled we print for each group the heatmap of the wells of the group
    if(generateHeatmapForWells) {
      std::string wellHeatMap = outputFolder + "_" + group + "_well.xlsx";
      createHeatmapOfWellsForGroup(reportingSettings, wellHeatMap, group, wellOrderMap, sizeX, sizeY, value);
    }

    // Each column represents one channel. Each channel is printed to a separate worksheet
    for(const auto &[channelIdx, results] : value.getChannels()) {
      if(results.getChannelMeta().name == "INVALID") {
        break;
      }
      // Write plate
      if(!sheets.contains(channelIdx)) {
        sheets[channelIdx] = workbook_add_worksheet(workbook, results.getChannelMeta().name.data());
        rowOffset          = ROW_OFFSET_START;

        for(auto const &[measureCh, measureChMeta] : results.getMeasuredChannels()) {
          if(reportingSettings.detail.measureChannels.contains(measureCh.getMeasureChannel())) {
            auto *sheet = sheets.at(channelIdx);

            auto writePlateFrame = [&sheet, &rowOffset, &row, &col, &numberFormat, &header](const std::string &name) {
              worksheet_merge_range(sheet, rowOffset - 1, 0, rowOffset - 1, PLATE_COLS + 1, "-", NULL);
              worksheet_write_string(sheet, rowOffset - 1, 0, name.data(), NULL);
              paintPlateBorder(sheet, PLATE_ROWS, PLATE_COLS, rowOffset, header, numberFormat);
            };

            writePlateFrame(measureChMeta.name);
            rowOffset = rowOffset + PLATE_ROWS + ROW_OFFSET_START + 4;
          }
        }
      }

      // Write data
      auto *sheet = sheets.at(channelIdx);
      try {
        if(row >= 0 && col >= 0) {
          rowOffset = ROW_OFFSET_START;
          for(const auto &[objKey, obj] : results.getObjects()) {
            for(const auto &[measKey, val] : obj.getMeasurements()) {
              if(reportingSettings.detail.measureChannels.contains(measKey.getMeasureChannel())) {
                std::string jobName = allOverReport.getJobMeta().jobName;

                auto writeNumber = [&sheet, &rowOffset, &row, &col, &numberFormat, &numberFormatInvalid,
                                    &generateHeatmapForWells, &jobName,
                                    &group = group](double value, bool areThereInvalidImagesInside) {
                  if(generateHeatmapForWells) {
                    std::string filePath = "external:.\\heatmaps/heatmap_" + group + "_" + jobName + ".xlsx";
                    worksheet_write_url(sheet, rowOffset + row, col, filePath.data(), NULL);
                  }
                  auto *format = numberFormat;
                  if(areThereInvalidImagesInside) {
                    format = numberFormatInvalid;
                  }
                  worksheet_write_number(sheet, rowOffset + row, col, value, format);
                };

                if(std::holds_alternative<double>(val.getVal())) {
                  writeNumber(std::get<double>(val.getVal()), obj.getObjectMeta().valid);
                }
                rowOffset = rowOffset + PLATE_ROWS + ROW_OFFSET_START + 4;
              }
            }
          }
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

}    // namespace joda::pipeline::reporting
