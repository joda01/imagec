
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
#include <regex>
#include <stdexcept>
#include <string>
#include "backend/duration_count/duration_count.h"

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
        .setColumnNames({{static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE), "#confidence"},
                         {static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE), "#areaSize"},
                         {static_cast<int>(ColumnIndexDetailedReport::PERIMETER), "#perimeter"},
                         {static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY), "#circularity"},
                         {static_cast<int>(ColumnIndexDetailedReport::VALIDITY), "#validity"},
                         {static_cast<int>(ColumnIndexDetailedReport::INTENSITY), "#intensity"},
                         {static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN), "#Min"},
                         {static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX), "#Max"}});
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
                                     const std::string &detailReportOutputPath, int tempChannelIdx, uint32_t tileIdx)
{
  try {
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
    DurationCount::stop(id);

    for(const auto &imgData : result.result) {
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE), imgData.getIndex(),
                                    imgData.getConfidence(), imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE), imgData.getIndex(),
                                    imgData.getAreaSize(), imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::PERIMETER), imgData.getIndex(),
                                    imgData.getPerimeter(), imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY), imgData.getIndex(),
                                    imgData.getCircularity(), imgData.getValidity());
      detailReportTable.getTableAt(tempChannelIdx, "")
          .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::VALIDITY), imgData.getIndex(),
                                    imgData.getValidity());

      int idxOffset = 0;
      for(const auto &[channelIndexIn, intensity] : imgData.getIntensity()) {
        int channelIndex = channelIndexIn;
        if(channelIndex < 0) {
          channelIndex = tempChannelIdx;
        }
        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY) + idxOffset,
                                      imgData.getIndex(), intensity.intensity, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::INTENSITY) + idxOffset,
                           "#intensity avg " + mAnalyzeSettings.getChannelNameOfIndex(channelIndex));

        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN) + idxOffset,
                                      imgData.getIndex(), intensity.intensityMax, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN) + idxOffset,
                           "#intensity min " + mAnalyzeSettings.getChannelNameOfIndex(channelIndex));

        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX) + idxOffset,
                                      imgData.getIndex(), intensity.intensityMin, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX) + idxOffset,
                           "#intensity max " + mAnalyzeSettings.getChannelNameOfIndex(channelIndex));
        idxOffset += 3;    // intnsity avg, min and max are 3 columns
      }
    }
  } catch(const std::exception &ex) {
    std::cout << "Pipeline::appendToDetailReport >" << ex.what() << "<" << std::endl;
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
  const int32_t CELL_SIZE        = 35;

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
  format_set_num_format(numberFormat, "0.00");
  format_set_font_size(numberFormat, 10);
  format_set_align(numberFormat, LXW_ALIGN_CENTER);
  format_set_align(numberFormat, LXW_ALIGN_VERTICAL_CENTER);

  int32_t rowOffset = 0;

  ///////////////////////////7
  lxw_conditional_format *condFormat = new lxw_conditional_format();
  condFormat->type                   = LXW_CONDITIONAL_3_COLOR_SCALE;
  condFormat->format                 = numberFormat;
  condFormat->min_color              = 0x63BE7B;
  condFormat->min_rule_type          = LXW_CONDITIONAL_RULE_TYPE_MINIMUM;
  condFormat->mid_color              = 0xFFEB84;
  condFormat->mid_rule_type          = LXW_CONDITIONAL_RULE_TYPE_PERCENTILE;
  condFormat->max_color              = 0xF8696B;
  condFormat->max_rule_type          = LXW_CONDITIONAL_RULE_TYPE_MAXIMUM;

  ///////////////////

  auto paintPlateBorder = [&header, &condFormat](lxw_worksheet *sheet, int32_t rowOffset) {
    // Column
    worksheet_set_column_pixels(sheet, 0, 0, HEADER_CELL_SIZE, NULL);
    worksheet_set_column_pixels(sheet, PLATE_COLS + 1, PLATE_COLS + 1, HEADER_CELL_SIZE, NULL);

    for(int col = 1; col < PLATE_COLS + 1; col++) {
      worksheet_set_column_pixels(sheet, col, col, CELL_SIZE, NULL);
      worksheet_write_string(sheet, rowOffset, col, std::to_string(col).data(), header);
      worksheet_write_string(sheet, PLATE_ROWS + rowOffset + 1, col, std::to_string(col).data(), header);
    }

    // Row
    worksheet_set_row_pixels(sheet, rowOffset, HEADER_CELL_SIZE, NULL);
    for(int row = 1; row < PLATE_ROWS + 1; row++) {
      char toWrt[2];
      toWrt[0] = (row - 1) + 'A';
      toWrt[1] = 0;

      worksheet_set_row_pixels(sheet, row + rowOffset, CELL_SIZE, NULL);
      worksheet_write_string(sheet, row + rowOffset, 0, toWrt, header);
      worksheet_write_string(sheet, row + rowOffset, PLATE_COLS + 1, toWrt, header);
    }

    worksheet_conditional_format_range(sheet, 1 + rowOffset, 1, 1 + rowOffset + PLATE_ROWS, 1 + PLATE_COLS, condFormat);
  };

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
        paintPlateBorder(sheets.at(channelIdx), rowOffset);
        rowOffset = PLATE_ROWS + ROW_OFFSET_START + 4;
        worksheet_merge_range(sheets.at(channelIdx), rowOffset - 1, 0, rowOffset - 1, PLATE_COLS + 1, "-", NULL);
        worksheet_write_string(sheets.at(channelIdx), rowOffset - 1, 0, "Intensity", NULL);
        paintPlateBorder(sheets.at(channelIdx), rowOffset);
        rowOffset = 2 * PLATE_ROWS + ROW_OFFSET_START + ROW_OFFSET_START + 6;
        worksheet_merge_range(sheets.at(channelIdx), rowOffset - 1, 0, rowOffset - 1, PLATE_COLS + 1, "-", NULL);
        worksheet_write_string(sheets.at(channelIdx), rowOffset - 1, 0, "Area size", NULL);
        paintPlateBorder(sheets.at(channelIdx), rowOffset);
      }

      auto *sheet = sheets.at(channelIdx);

      try {
        auto row = applyRegex(mAnalyzeSettings.getReportingSettings().getFileRegex(), group).row;
        auto col = applyRegex(mAnalyzeSettings.getReportingSettings().getFileRegex(), group).col;

        rowOffset = ROW_OFFSET_START;
        worksheet_write_number(
            sheet, rowOffset + row, col,
            values.getStatistics().at(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE)).getSum(), numberFormat);

        rowOffset = PLATE_ROWS + ROW_OFFSET_START + 4;
        worksheet_write_number(
            sheet, rowOffset + row, col,
            values.getStatistics().at(static_cast<int>(ColumnIndexDetailedReport::INTENSITY)).getSum(), numberFormat);

        rowOffset = 2 * PLATE_ROWS + ROW_OFFSET_START + ROW_OFFSET_START + 6;
        worksheet_write_number(
            sheet, rowOffset + row, col,
            values.getStatistics().at(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE)).getSum(), numberFormat);
      } catch(const std::exception &) {
      }
    }
  }

  workbook_close(workbook);
}

///
/// \brief      Depending on the settings images could be stored in folder, filename or no group
/// \author     Joachim Danmayr
///
auto Reporting::getGroupToStoreImageIn(const std::string &imagePath, const std::string &imageName) -> std::string
{
  switch(mAnalyzeSettings.getReportingSettings().getGroupBy()) {
    case settings::json::AnalyzeSettingsReporting::GroupBy::NONE:
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
