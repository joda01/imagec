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

#include "results_helper.hpp"
#include <exception>
#include <mutex>
#include <string>
#include "backend/duration_count/duration_count.h"
#include "backend/helper/fnv1a.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/roi/roi.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/logger/console_logger.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/results/results.hpp"
#include "backend/results/results_defines.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_reporting_settings.hpp"
#include "backend/settings/experiment_settings.hpp"
#include "backend/settings/settings.hpp"
#include "results_math.hpp"

namespace joda::results {

using MeasureChannels    = joda::settings::ChannelReportingSettings::MeasureChannels;
using MeasureChannelStat = joda::settings::ChannelReportingSettings::MeasureChannelStat;

///
/// \brief      Set detail report header
/// \author     Joachim Danmayr
///
void Helper::setDetailReportHeader(const joda::settings::AnalyzeSettings &analyzeSettings,
                                   joda::results::WorkSheet &detailReportTable, const std::string &channelName,
                                   joda::settings::ChannelIndex chIdx)
{
  try {
    detailReportTable[chIdx].setName(channelName);
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::CONFIDENCE, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::AREA_SIZE, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::PERIMETER, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::CIRCULARITY, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::VALIDITY, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::INVALIDITY, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::CENTER_OF_MASS_X, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::CENTER_OF_MASS_Y, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::INTENSITY_AVG, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::INTENSITY_MIN, chIdx});
    detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::INTENSITY_MAX, chIdx});

    //
    // Intensity channels
    //
    for(joda::settings::ChannelIndex intensIdx :
        joda::settings::Settings::getCrossChannelSettingsForChannel(analyzeSettings, chIdx)
            .crossChannelIntensityChannels) {
      detailReportTable[chIdx].emplaceMeasureChKey(
          MeasureChannelKey{MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL, intensIdx});

      detailReportTable[chIdx].emplaceMeasureChKey(
          MeasureChannelKey{MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL, intensIdx});

      detailReportTable[chIdx].emplaceMeasureChKey(
          MeasureChannelKey{MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL, intensIdx});
    }

    //
    // Counting channels
    //
    for(joda::settings::ChannelIndex countIdx :
        joda::settings::Settings::getCrossChannelSettingsForChannel(analyzeSettings, chIdx).crossChannelCountChannels) {
      detailReportTable[chIdx].emplaceMeasureChKey(MeasureChannelKey{MeasureChannels::COUNT_CROSS_CHANNEL, countIdx});
    }

  } catch(const std::exception &ex) {
    std::cout << "Pipeline::setDetailReportHeader >" << ex.what() << "<" << std::endl;
  }
}

///
/// \brief      Append to detailed report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
/// \param[in]  result The results of a channel
///
void Helper::appendToDetailReport(const joda::settings::AnalyzeSettings &analyzeSettings,
                                  const joda::func::DetectionResponse &result,
                                  joda::results::WorkSheet &detailReportTable,
                                  const std::string &detailReportOutputPath, const std::string &jobName,
                                  joda::settings::ChannelIndex chIdx, uint32_t tileIdx, const ImageProperties &imgProps)
{
  static std::mutex appendMutex;

  static const std::string separator(1, std::filesystem::path::preferred_separator);

  // Free memory
  auto id = DurationCount::start("Write control image");
  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(0);

  if(!result.controlImage.empty()) {
    cv::imwrite(detailReportOutputPath + separator + "control_" + joda::settings::to_string(chIdx) + "_" +
                    std::to_string(tileIdx) + "_" + jobName + ".png",
                result.controlImage, compression_params);
  } else {
    std::cout << "CTRL img null" << std::endl;
  }
  DurationCount::stop(id);
  // if(!result.originalImage.empty()) {
  //   cv::imwrite(detailReportOutputPath + separator + "original_" + std::to_string(tempChannelIdx) + "_" +
  //                   std::to_string(tileIdx) + ".png",
  //               result.originalImage * ((float) UINT8_MAX / (float) UINT16_MAX), compression_params);
  // }

  results::Channel &tableToWorkOn = detailReportTable[chIdx];
  tableToWorkOn.setValidity(result.responseValidity, result.invalidateWholeImage);
  int64_t indexOffset = 0;
  {
    std::lock_guard<std::mutex> lock(appendMutex);
    indexOffset = tableToWorkOn.getNrOfObjects();
  }
  auto [offsetX, offsetY] =
      TiffLoader::calculateTileXYoffset(joda::algo::TILES_TO_LOAD_PER_RUN, tileIdx, imgProps.width, imgProps.height,
                                        imgProps.tileWidth, imgProps.tileHeight);
  int64_t xMul    = offsetX * imgProps.tileWidth;
  int64_t yMul    = offsetY * imgProps.tileHeight;
  uint64_t roiIdx = 0;
  for(const auto &roi : result.result) {
    try {
      // int64_t index = roi.getIndex() + indexOffset;
      uint64_t index = roiIdx + indexOffset;
      tableToWorkOn[index].setValidity(roi.getValidity());
      tableToWorkOn[index][{MeasureChannels::CONFIDENCE, chIdx}]       = roi.getConfidence();
      tableToWorkOn[index][{MeasureChannels::AREA_SIZE, chIdx}]        = roi.getAreaSize();
      tableToWorkOn[index][{MeasureChannels::PERIMETER, chIdx}]        = roi.getPerimeter();
      tableToWorkOn[index][{MeasureChannels::CIRCULARITY, chIdx}]      = roi.getCircularity();
      tableToWorkOn[index][{MeasureChannels::VALIDITY, chIdx}]         = roi.getValidity();
      tableToWorkOn[index][{MeasureChannels::INVALIDITY, chIdx}]       = roi.getValidity();
      tableToWorkOn[index][{MeasureChannels::CENTER_OF_MASS_X, chIdx}] = roi.getCenterOfMass().x + xMul;
      tableToWorkOn[index][{MeasureChannels::CENTER_OF_MASS_Y, chIdx}] = roi.getCenterOfMass().y + yMul;

      double intensityAvg = 0;
      double intensityMin = 0;
      double intensityMax = 0;
      if(roi.getIntensity().contains(chIdx)) {
        auto intensityMe = roi.getIntensity().at(chIdx);
        intensityAvg     = intensityMe.intensity;
        intensityMin     = intensityMe.intensityMin;
        intensityMax     = intensityMe.intensityMax;
      }
      tableToWorkOn[index][{MeasureChannels::INTENSITY_AVG, chIdx}] = intensityAvg;
      tableToWorkOn[index][{MeasureChannels::INTENSITY_MIN, chIdx}] = intensityMin;
      tableToWorkOn[index][{MeasureChannels::INTENSITY_MAX, chIdx}] = intensityMax;

      //
      // Intensity channels
      //
      for(const auto &[idx, intensity] : roi.getIntensity()) {
        if(idx != chIdx) {
          tableToWorkOn[index][{MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL, idx}] = intensity.intensity;
          tableToWorkOn[index][{MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL, idx}] = intensity.intensityMin;
          tableToWorkOn[index][{MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL, idx}] = intensity.intensityMax;
        }
      }

      //
      // Counting channels
      //
      for(const auto &[idx, intersecting] : roi.getIntersectingRois()) {
        tableToWorkOn[index][{MeasureChannels::COUNT_CROSS_CHANNEL, idx}] = intersecting.roiValid.size();
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
void Helper::appendToAllOverReport(const joda::settings::AnalyzeSettings &analyzeSettings,
                                   joda::results::WorkSheet &allOverReport,
                                   const joda::results::WorkSheet &detailReportIn, const std::string &imagePath,
                                   const std::string &imageName, int nrOfChannels)
{
  std::lock_guard<std::mutex> lock(mAppendToAllOverReportMutex);

  try {
    std::string groupToStoreImageIn            = getGroupToStoreImageIn(analyzeSettings, imagePath, imageName);
    const joda::results::Group &detailedReport = detailReportIn.root();
    joda::results::Group &groupToWorkOn        = allOverReport[groupToStoreImageIn];

    bool invalidAll = false;
    if(detailedReport.containsInvalidChannelWhereOneInvalidatesTheWholeImage()) {
      invalidAll = true;
    }

    for(const auto &[channelIdx, detailChannel] : detailedReport.getChannels()) {
      results::Channel &allOverChannelToWorkOn = groupToWorkOn[channelIdx];
      allOverChannelToWorkOn.setName(detailChannel.getName());

      for(const auto &measureIdx : detailChannel.getMeasuredChannels()) {
        allOverChannelToWorkOn.emplaceMeasureChKey({measureIdx, MeasureChannelStat::AVG});
        allOverChannelToWorkOn.emplaceMeasureChKey({measureIdx, MeasureChannelStat::SUM});
        allOverChannelToWorkOn.emplaceMeasureChKey({measureIdx, MeasureChannelStat::MIN});
        allOverChannelToWorkOn.emplaceMeasureChKey({measureIdx, MeasureChannelStat::MAX});
        allOverChannelToWorkOn.emplaceMeasureChKey({measureIdx, MeasureChannelStat::CNT});
        allOverChannelToWorkOn.emplaceMeasureChKey({measureIdx, MeasureChannelStat::STD_DEV});
      }
      auto stats = calcStats(detailChannel);

      Object &imageToWorkOn = allOverChannelToWorkOn[fnv1a(imagePath)];
      imageToWorkOn.setNameAndRef(imageName, imagePath);

      for(const auto &[measureKey, val] : stats.measurements) {
        imageToWorkOn.measurements[measureKey].set(val);
      }
    }
  } catch(const std::exception &ex) {
    joda::log::logWarning("Pipeline::appendToAllOverReport >" + std::string(ex.what()) + "<!");
  }
}

///
/// \brief      Depending on the settings images could be stored in folder, filename or no group
/// \author     Joachim Danmayr
///
auto Helper::getGroupToStoreImageIn(const joda::settings::AnalyzeSettings &analyzeSettings,
                                    const std::string &imagePath, const std::string &imageName) -> std::string
{
  switch(analyzeSettings.experimentSettings.groupBy) {
    case settings::ExperimentSettings::GroupBy::OFF:
      return {};
    case settings::ExperimentSettings::GroupBy::DIRECTORY:
      return imagePath;
    case settings::ExperimentSettings::GroupBy::FILENAME:
      try {
        return applyRegex(analyzeSettings.experimentSettings.filenameRegex, imageName).group;
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

}    // namespace joda::results
