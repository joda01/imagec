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
#include <exception>
#include <mutex>
#include <string>
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
                                   int realChannelIdx, int /*tempChannelIdx*/, const std::set<int32_t> &colocGroup)
{
  try {
    int channelIndexOffset = 0;

    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "confidence",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "areaSize",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "perimeter",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::PERIMETER));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "circularity",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "validity",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::VALIDITY));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "invalidity",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::INVALIDITY));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "x",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_X));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "y",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_Y));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "intensity avg",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::INTENSITY_AVG));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "intensity min",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN));
    channelIndexOffset++;
    detailReportTable.getTableAt(realChannelIdx, channelName)
        .setColumnName(channelIndexOffset, "intensity max",
                       realChannelIdx | static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX));
    channelIndexOffset++;

    std::set<int32_t> intensityGroups;
    if(!colocGroup.empty()) {
      // This is an intersection channel. Intersection channels do have intensity for each channel they calculate the
      // intersection for
      intensityGroups = colocGroup;
    } else {
      intensityGroups = analyzeSettings.getNumberOfCrossChannelIntensityMeasurementForChannel(realChannelIdx);
    }

    //
    // Intensity channels
    //
    for(int intensIdx : intensityGroups) {
      detailReportTable.getTableAt(realChannelIdx, channelName)
          .setColumnName(channelIndexOffset, "intensity avg " + analyzeSettings.getChannelNameOfIndex(intensIdx),
                         intensIdx | static_cast<int>(ColumnIndexDetailedReport::INTENSITY_AVG_CROSS_CHANNEL));

      channelIndexOffset++;

      detailReportTable.getTableAt(realChannelIdx, channelName)
          .setColumnName(channelIndexOffset, "intensity min " + analyzeSettings.getChannelNameOfIndex(intensIdx),
                         intensIdx | static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN_CROSS_CHANNEL));

      channelIndexOffset++;

      detailReportTable.getTableAt(realChannelIdx, channelName)
          .setColumnName(channelIndexOffset, "intensity max " + analyzeSettings.getChannelNameOfIndex(intensIdx),
                         intensIdx | static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX_CROSS_CHANNEL));
      channelIndexOffset++;
    }

    //
    // Counting channels
    //
    for(int countIdx : analyzeSettings.getNumberOfCrossChannelCountMeasurementForChannel(realChannelIdx)) {
      detailReportTable.getTableAt(realChannelIdx, channelName)
          .setColumnName(channelIndexOffset, "count " + analyzeSettings.getChannelNameOfIndex(countIdx),
                         countIdx | static_cast<int>(ColumnIndexDetailedReport::INTERSECTION_CROSS_CHANNEL));

      channelIndexOffset++;
    }

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
                                  int realChannelIdx, int /*tempChannelIdx*/, uint32_t tileIdx,
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
    cv::imwrite(detailReportOutputPath + separator + "control_" + std::to_string(realChannelIdx) + "_" +
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
    indexOffset = detailReportTable.getTableAt(realChannelIdx, "")
                      .getNrOfRowsAtColumn(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
  }
  int64_t roiIdx = 0;
  for(const auto &roi : result.result) {
    try {
      // int64_t index = roi.getIndex() + indexOffset;
      int64_t index = roiIdx + indexOffset;

      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE) | realChannelIdx,
                                           index, roi.getConfidence(), roi.getValidity());
      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE) | realChannelIdx,
                                           index, roi.getAreaSize(), roi.getValidity());
      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::PERIMETER) | realChannelIdx,
                                           index, roi.getPerimeter(), roi.getValidity());
      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY) | realChannelIdx,
                                           index, roi.getCircularity(), roi.getValidity());

      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::VALIDITY) | realChannelIdx,
                                           index, roi.getValidity(), roi.getValidity());

      //
      bool isValid = roi.getValidity() == func::ParticleValidity::VALID;
      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(
              static_cast<int>(ColumnIndexDetailedReport::INVALIDITY) | realChannelIdx, index,
              isValid ? func::ParticleValidity::INVALID : func::ParticleValidity::VALID, roi.getValidity());

      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_X) |
                                               realChannelIdx,
                                           index, roi.getCenterOfMass().x + xMul, roi.getValidity());
      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::CENTER_OF_MASS_Y) |
                                               realChannelIdx,
                                           index, roi.getCenterOfMass().y + yMul, roi.getValidity());

      double intensityAvg = 0;
      double intensityMin = 0;
      double intensityMax = 0;
      bool notc           = false;
      if(roi.getIntensity().contains(realChannelIdx)) {
        auto intensityMe = roi.getIntensity().at(realChannelIdx);

        intensityAvg = intensityMe.intensity;
        intensityMin = intensityMe.intensityMin;
        intensityMax = intensityMe.intensityMax;
      }
      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_AVG) | realChannelIdx,
                                           index, intensityAvg, roi.getValidity());
      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN) | realChannelIdx,
                                           index, intensityMin, roi.getValidity());
      detailReportTable.getTableAt(realChannelIdx, "")
          .appendValueToColumnAtRowWithKey(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX) | realChannelIdx,
                                           index, intensityMax, roi.getValidity());

      //
      // Intensity channels
      //
      for(const auto &[idx, intensity] : roi.getIntensity()) {
        if(idx != realChannelIdx) {
          if(!detailReportTable.getTableAt(realChannelIdx, "")
                  .columnKeyExists(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_AVG_CROSS_CHANNEL) | idx)) {
            continue;
          }
          detailReportTable.getTableAt(realChannelIdx, "")
              .appendValueToColumnAtRowWithKey(
                  static_cast<int>(ColumnIndexDetailedReport::INTENSITY_AVG_CROSS_CHANNEL) | idx, index,
                  intensity.intensity, roi.getValidity());

          if(!detailReportTable.getTableAt(realChannelIdx, "")
                  .columnKeyExists(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN_CROSS_CHANNEL) | idx)) {
            continue;
          }
          detailReportTable.getTableAt(realChannelIdx, "")
              .appendValueToColumnAtRowWithKey(
                  static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN_CROSS_CHANNEL) | idx, index,
                  intensity.intensityMin, roi.getValidity());

          if(!detailReportTable.getTableAt(realChannelIdx, "")
                  .columnKeyExists(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX_CROSS_CHANNEL) | idx)) {
            continue;
          }
          detailReportTable.getTableAt(realChannelIdx, "")
              .appendValueToColumnAtRowWithKey(
                  static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX_CROSS_CHANNEL) | idx, index,
                  intensity.intensityMax, roi.getValidity());
        }
      }

      //
      // Counting channels
      //
      for(const auto &[idx, intersecting] : roi.getIntersectingRois()) {
        int64_t colKey = static_cast<int>(ColumnIndexDetailedReport::INTERSECTION_CROSS_CHANNEL) | idx;
        if(!detailReportTable.getTableAt(realChannelIdx, "").columnKeyExists(colKey)) {
          continue;
        }
        detailReportTable.getTableAt(realChannelIdx, "")
            .appendValueToColumnAtRowWithKey(colKey, index, intersecting.roiValid.size(),
                                             joda::func::ParticleValidity::VALID);
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
  std::lock_guard<std::mutex> lock(mAppendToAllOverReportMutex);

  const int NR_OF_COLUMNS_PER_CHANNEL = 7;
  try {
    std::string groupToStoreImageIn                  = getGroupToStoreImageIn(analyzeSettings, imagePath, imageName);
    joda::results::ReportingContainer &tableToWorkOn = allOverReport[groupToStoreImageIn];

    for(const auto &[channelIdx, _] : detailedReport.mColumns) {
      // if(!detailedReport.containsTable(tempChannelIdx)) {
      //   joda::log::logWarning("Does not container channel >" + std::to_string(tempChannelIdx) + "<!");
      //   continue;
      // }

      std::string tableName = detailedReport.getTableAt(channelIdx).getTableName();
      uint32_t nrOfCols     = detailedReport.getTableAt(channelIdx).getNrOfColumns();

      int rowIdx = 0;
      for(int colIdx = 0; colIdx < nrOfCols; colIdx++) {
        tableToWorkOn.getTableAt(channelIdx, tableName)
            .setColumnName(colIdx, detailedReport.getTableAt(channelIdx).getColumnNameAt(colIdx),
                           detailedReport.getTableAt(channelIdx).getColumnKeyAt(colIdx));

        if(detailedReport.getTableAt(channelIdx).containsStatistics(colIdx)) {
          auto colStatistics = detailedReport.getTableAt(channelIdx).getStatistics(colIdx);

          auto val  = colStatistics.getAvg();
          auto mask = (tableToWorkOn.getTableAt(channelIdx, tableName).getColumnKeyAt(colIdx) & COLUMN_MASK);
          if(mask == (uint32_t) ColumnIndexDetailedReport::VALIDITY ||
             mask == (uint32_t) ColumnIndexDetailedReport::INVALIDITY) {
            val = colStatistics.getSum();
          }

          rowIdx = tableToWorkOn.getTableAt(channelIdx, tableName)
                       .appendValueToColumn(colIdx, val, joda::func::ParticleValidity::VALID);
        } else {
          // No statistics, just add NaN
          rowIdx = tableToWorkOn.getTableAt(channelIdx, tableName)
                       .appendValueToColumn(colIdx, std::numeric_limits<double>::quiet_NaN(),
                                            joda::func::ParticleValidity::UNKNOWN);
        }
      }

      // This tells the table how many rows are available
      tableToWorkOn.getTableAt(channelIdx, tableName).setRowName(rowIdx, imageName);
    }
  } catch(const std::exception &ex) {
    joda::log::logWarning("Pipeline::appendToAllOverReport >" + std::string(ex.what()) + "<!");
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