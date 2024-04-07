///
/// \file      reporting_helper.cpp
/// \author    Joachim Danmayr
/// \date      2024-04-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Reporting helper
///

#include "reporting_helper.hpp"
#include <mutex>
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/roi/roi.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/logger/console_logger.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/settings/analze_settings_parser.hpp"

namespace joda::pipeline::reporting {

///
/// \brief      Set detail report header
/// \author     Joachim Danmayr
///
void Helper::setDetailReportHeader(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                   joda::results::ReportingContainer &detailReportTable, const std::string &channelName,
                                   int tempChannelIdx)
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
                         {static_cast<int>(ColumnIndexDetailedReport::DYNAMIC), "intensity"},
                         {static_cast<int>(ColumnIndexDetailedReport::DYNAMIC_MIN), "intensity min"},
                         {static_cast<int>(ColumnIndexDetailedReport::DYNAMIC_MAX), "intensity max"}});
  } catch(const std::exception &ex) {
    std::cout << "Pipeline::setDetailReportHeader >" << ex.what() << "<" << std::endl;
  }
}

///
/// \brief      Append to detailed report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
///
void Helper::appendToDetailReport(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                  const joda::func::DetectionResponse &result,
                                  joda::results::ReportingContainer &detailReportTable,
                                  const std::string &detailReportOutputPath, const std::string &jobName,
                                  int realChannelIdx, int tempChannelIdx, uint32_t tileIdx,
                                  const ImageProperties &imgProps)
{
  static std::mutex appendMutex;

  static const std::string separator(1, std::filesystem::path::preferred_separator);

  // Free memory
  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(0);

  auto id = DurationCount::start("write-control-image");
  if(!result.controlImage.empty()) {
    cv::imwrite(detailReportOutputPath + separator + "control_" + std::to_string(tempChannelIdx) + "_" +
                    std::to_string(tileIdx) + "_" + jobName + ".png",
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

  DurationCount::stop(id);
  int64_t indexOffset = 0;
  {
    std::lock_guard<std::mutex> lock(appendMutex);
    indexOffset = detailReportTable.getTableAt(tempChannelIdx, "")
                      .getNrOfRowsAtColumn(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
  }
  int64_t roiIdx = 0;
  for(const auto &imgData : result.result) {
    try {
      // int64_t index = imgData.getIndex() + indexOffset;
      int64_t index = roiIdx + indexOffset;

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

      //
      // Intensity channels
      //
      int intensityOffset = 0;
      for(const auto &[channelIndexIn, intensity] : imgData.getIntensity()) {
        int channelIndex = channelIndexIn;
        if(channelIndex < 0) {
          channelIndex = realChannelIdx;
        }
        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC) + intensityOffset, index,
                                      intensity.intensity, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC) + intensityOffset,
                           "intensity avg " + analyzeSettings.getChannelNameOfIndex(channelIndex));

        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC_MIN) + intensityOffset, index,
                                      intensity.intensityMax, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC_MIN) + intensityOffset,
                           "intensity min " + analyzeSettings.getChannelNameOfIndex(channelIndex));

        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC_MAX) + intensityOffset, index,
                                      intensity.intensityMin, imgData.getValidity());
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC_MAX) + intensityOffset,
                           "intensity max " + analyzeSettings.getChannelNameOfIndex(channelIndex));
        intensityOffset += 3;    // intnsity avg, min and max are 3 columns
      }

      //
      // Counting channels
      //
      for(const auto &[channelIndexIn, intersecting] : imgData.getIntersectingRois()) {
        int channelIndex = channelIndexIn;
        if(channelIndex < 0) {
          channelIndex = realChannelIdx;
        }
        detailReportTable.getTableAt(tempChannelIdx, "")
            .setColumnName(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC) + intensityOffset,
                           "count " + analyzeSettings.getChannelNameOfIndex(channelIndex));

        detailReportTable.getTableAt(tempChannelIdx, "")
            .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC) + intensityOffset, index,
                                      intersecting.roiValid.size(), joda::func::ParticleValidity::VALID);

        intensityOffset++;
      }

      roiIdx++;

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
void Helper::appendToAllOverReport(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                   std::map<std::string, joda::results::ReportingContainer> &allOverReport,
                                   const joda::results::ReportingContainer &detailedReport,
                                   const std::string &imagePath, const std::string &imageName, int nrOfChannels)
{
  const int NR_OF_COLUMNS_PER_CHANNEL = 7;
  try {
    for(int tempChannelIdx = 0; tempChannelIdx < nrOfChannels; tempChannelIdx++) {
      if(!detailedReport.containsTable(tempChannelIdx)) {
        continue;
      }

      allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
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
                      .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC))},
          });

      /// \todo Copy constructor is called here
      auto colStatistics = detailedReport.getTableAt(tempChannelIdx)
                               .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
      allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(0, colStatistics.getNr(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
      allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(1, colStatistics.getInvalid(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
      allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(2, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE));
      allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(3, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::PERIMETER));
      allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(4, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

      //
      colStatistics = detailedReport.getTableAt(tempChannelIdx)
                          .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY));
      allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .appendValueToColumn(5, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

      //
      int64_t column = 6;
      int stasOffset = 0;
      int rowIdx     = 0;
      while(detailedReport.getTableAt(tempChannelIdx)
                .containsStatistics(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC) + stasOffset)) {
        colStatistics = detailedReport.getTableAt(tempChannelIdx)
                            .getStatistics(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC) + stasOffset);

        rowIdx = allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
                     .getTableAt(tempChannelIdx, "")
                     .appendValueToColumn(column, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

        allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
            .getTableAt(tempChannelIdx, "")
            .setColumnName(column,
                           detailedReport.getTableAt(tempChannelIdx)
                               .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::DYNAMIC) + stasOffset));

        // stasOffset += 3;    // intensity avg, min, max
        stasOffset++;
        column++;
      }
      // This tells the table how many rows are available
      allOverReport[getGroupToStoreImageIn(analyzeSettings, imagePath, imageName)]
          .getTableAt(tempChannelIdx, "")
          .setRowName(rowIdx, imageName);
    }
  } catch(const std::exception &ex) {
    std::cout << "Pipeline::appendToAllOverReport >" << ex.what() << "<" << std::endl;
  }
}

///
/// \brief      Depending on the settings images could be stored in folder, filename or no group
/// \author     Joachim Danmayr
///
auto Helper::getGroupToStoreImageIn(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                    const std::string &imagePath, const std::string &imageName) -> std::string
{
  switch(analyzeSettings.getReportingSettings().getHeatmapSettings().getGroupBy()) {
    case settings::json::AnalyzeSettingsReportingHeatmap::GroupBy::OFF:
      return {};
    case settings::json::AnalyzeSettingsReportingHeatmap::GroupBy::FOLDER:
      return imagePath;
    case settings::json::AnalyzeSettingsReportingHeatmap::GroupBy::FILENAME:
      try {
        return applyRegex(analyzeSettings.getReportingSettings().getHeatmapSettings().getFileRegex(), imageName).group;
      } catch(const std::exception &) {
        return "INVALID";
      }
  }
  return "INVALID";
}

///
/// \brief      Apply regex
/// \author     Joachim Danmayr
///
Helper::RegexResult Helper::applyRegex(const std::string &regex, const std::string &fileName)
{
  std::regex pattern(regex);
  std::smatch match;
  Helper::RegexResult result;

  if(std::regex_search(fileName, match, pattern)) {
    if(match.size() >= 5) {
      result.row = stringToNumber(match[2].str());
      result.col = stringToNumber(match[3].str());
      result.img = stringToNumber(match[4].str());

      char rowChar[2];
      rowChar[0]   = result.row - 1 + 'A';
      rowChar[1]   = 0;
      result.group = "well_" + std::string(rowChar) + "_" + std::to_string(result.col);
    } else {
      throw std::invalid_argument("Pattern not found.");
    }
  } else {
    throw std::invalid_argument("Pattern not found.");
  }
  return result;
}

///
/// \brief      Apply regex to extract group row and col
/// \author     Joachim Danmayr
///
Helper::RegexResult Helper::applyGroupRegex(const std::string &fileName)
{
  std::regex pattern("well_([A-Z]+)_([0-9]+)");
  std::smatch match;
  Helper::RegexResult result;

  if(std::regex_search(fileName, match, pattern)) {
    if(match.size() >= 3) {
      result.group = match[0];
      result.row   = stringToNumber(match[1].str());
      result.col   = stringToNumber(match[2].str());
      result.img   = -1;
    } else {
      throw std::invalid_argument("Pattern not found.");
    }
  } else {
    throw std::invalid_argument("Pattern not found.");
  }
  return result;
}

///
/// \brief      Converts a a string to a number
/// \author     Joachim Danmayr
///
auto Helper::stringToNumber(const std::string &str) -> int
{
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

}    // namespace joda::pipeline::reporting
