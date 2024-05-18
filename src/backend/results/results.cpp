///
/// \file      results.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "results.hpp"
#include <regex.h>
#include <memory>
#include <regex>
#include <stdexcept>
#include "backend/helper/helper.hpp"
#include "backend/helper/uuid.hpp"
#include "backend/image_processing/reader/tif/image_loader_tif.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/results/database/database.hpp"
#include "measure_channels.hpp"

namespace joda::results {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Results::Results(const std::filesystem::path &resultsFolder, const ExperimentSetting &settings) :
    mExperimentSettings(settings), mJobId(joda::helper::generate_uuid())
{
  prepareOutputFolders(resultsFolder);
  mDatabase = std::make_shared<db::Database>(mDatabaseFileName);
  mDatabase->createJob(db::JobMeta{.experimentId = settings.experimentId,
                                   .jobId        = mJobId,
                                   .name         = settings.jobName,
                                   .scientists   = {settings.scientistName},
                                   .location     = "",
                                   .notes        = ""});

  mDatabase->createPlate(db::PlateMeta{.jobId = mJobId, .plateId = settings.plateIdx, .notes = ""});
}

///
///
/// \brief      Creates the output folder for the results and returns the path.
///             Outputfolder = <outputFolder>/<DATE-TIME>_<JOB-NAME>
/// \author     Joachim Danmayr
/// \param[in]  resultsFolder Folder where the results should be stored in
///
void Results::prepareOutputFolders(const std::filesystem::path &resultsFolder)
{
  auto nowString      = ::joda::helper::timeNowToString();
  mOutputFolder       = resultsFolder / (nowString + "_" + mExperimentSettings.jobName);
  mOutputFolderImages = mOutputFolder / CONTROL_IMAGE_PATH;
  mDatabaseFileName   = mOutputFolder / DB_FILENAME;

  if(!std::filesystem::exists(mOutputFolder)) {
    std::filesystem::create_directories(mOutputFolder);
  }

  if(!std::filesystem::exists(mOutputFolderImages)) {
    std::filesystem::create_directories(mOutputFolderImages);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Results::appendToDetailReport(const joda::image::detect::DetectionResults &results,
                                   const joda::settings::ChannelSettings &channelSettings, uint16_t tileIdx,
                                   const image::ImageProperties &imgProps, const std::filesystem::path &imagePath)
{
  auto wellPosition = applyRegex(mExperimentSettings.imageFileNameRegex, imagePath);
  mDatabase->createWell(db::WellMeta{.jobId    = mJobId,
                                     .plateId  = mExperimentSettings.plateIdx,
                                     .wellId   = wellPosition.well.wellId,
                                     .wellPosX = wellPosition.well.wellPos[RegexResult::POS_X],
                                     .wellPosY = wellPosition.well.wellPos[RegexResult::POS_Y],
                                     .notes    = ""});

  mDatabase->createImage(db::ImageMeta{.jobId     = mJobId,
                                       .plateId   = mExperimentSettings.plateIdx,
                                       .wellId    = wellPosition.well.wellId,
                                       .imageId   = wellPosition.imageId,
                                       .imageName = imagePath.filename().string(),
                                       .width     = imgProps.width,
                                       .height    = imgProps.height});

  uint8_t channelId = static_cast<uint8_t>(channelSettings.meta.channelIdx);
  mDatabase->createChannel(db::ChannelMeta{.jobId     = mJobId,
                                           .plateId   = mExperimentSettings.plateIdx,
                                           .wellId    = wellPosition.well.wellId,
                                           .imageId   = wellPosition.imageId,
                                           .channelId = channelId});

  auto [offsetX, offsetY] =
      ::joda::image::TiffLoader::calculateTileXYoffset(::joda::pipeline::TILES_TO_LOAD_PER_RUN, tileIdx, imgProps.width,
                                                       imgProps.height, imgProps.tileWidth, imgProps.tileHeight);
  int64_t xMul = offsetX * imgProps.tileWidth;
  int64_t yMul = offsetY * imgProps.tileHeight;

  duckdb::vector<duckdb::Value> measureChannelKeys{};
  db::objects_t objects;
  uint64_t roiIdx = 0;
  for(const auto &roi : results) {
    uint64_t index = roiIdx;
    roiIdx++;
    db::Data &chan = objects[index];

    chan.validity = toValidity(roi.getValidity());

    // Measure channels
    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::CONFIDENCE, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(roi.getConfidence()));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::AREA_SIZE, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(roi.getAreaSize()));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::PERIMETER, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(roi.getPerimeter()));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::CIRCULARITY, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(roi.getCircularity()));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::CENTER_OF_MASS_X, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getCenterOfMass().x) + xMul));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::CENTER_OF_MASS_Y, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getCenterOfMass().y) + yMul));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::BOUNDING_BOX_WIDTH, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getBoundingBox().width)));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::BOUNDING_BOX_HEIGHT, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getBoundingBox().height)));

    double intensityAvg = 0;
    double intensityMin = 0;
    double intensityMax = 0;
    if(roi.getIntensity().contains(channelSettings.meta.channelIdx)) {
      auto intensityMe = roi.getIntensity().at(channelSettings.meta.channelIdx);
      intensityAvg     = intensityMe.intensity;
      intensityMin     = intensityMe.intensityMin;
      intensityMax     = intensityMe.intensityMax;
    }

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::INTENSITY_AVG, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(intensityAvg));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::INTENSITY_MIN, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(intensityMin));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelKey(MeasureChannel::INTENSITY_MAX, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(intensityMax));

    //
    // Intensity channels
    //
    for(const auto &[idx, intensity] : roi.getIntensity()) {
      if(idx != channelSettings.meta.channelIdx) {
        chan.keys.emplace_back(duckdb::Value::UINTEGER(
            (uint32_t) MeasureChannelKey(MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, toMeasureChannelIndex(idx))));
        chan.vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensity));

        chan.keys.emplace_back(duckdb::Value::UINTEGER(
            (uint32_t) MeasureChannelKey(MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, toMeasureChannelIndex(idx))));
        chan.vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensityMin));

        chan.keys.emplace_back(duckdb::Value::UINTEGER(
            (uint32_t) MeasureChannelKey(MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, toMeasureChannelIndex(idx))));
        chan.vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensityMax));
      }
    }

    //
    // Counting channels
    //
    for(const auto &[idx, intersecting] : roi.getIntersectingRois()) {
      chan.keys.emplace_back(duckdb::Value::UINTEGER(
          (uint32_t) MeasureChannelKey(MeasureChannel::CROSS_CHANNEL_COUNT, toMeasureChannelIndex(idx))));
      chan.vals.emplace_back(duckdb::Value::DOUBLE(intersecting.roiValid.size()));
    }
  }

  mDatabase->createObjects(db::ObjectMeta{.jobId     = mJobId,
                                          .plateId   = mExperimentSettings.plateIdx,
                                          .wellId    = wellPosition.well.wellId,
                                          .imageId   = wellPosition.imageId,
                                          .channelId = static_cast<uint8_t>(channelSettings.meta.channelIdx),
                                          .tileId    = tileIdx,
                                          .objects   = objects});
}

///
/// \brief      Apply regex
/// \author     Joachim Danmayr
///
Results::RegexResult Results::applyRegex(const std::string &regex, const std::filesystem::path &imagePath)
{
  std::regex pattern(regex);
  std::smatch match;
  RegexResult result;

  std::string fileName = imagePath.filename().string();
  if(std::regex_search(fileName, match, pattern)) {
    if(match.size() >= 5) {
      result.well.wellPos[RegexResult::POS_Y] = helper::stringToNumber(match[2].str());
      result.well.wellPos[RegexResult::POS_X] = helper::stringToNumber(match[3].str());
      result.imageId                          = helper::stringToNumber(match[4].str());

    } else {
      throw std::invalid_argument("Pattern not found.");
    }
  } else {
    throw std::invalid_argument("Pattern not found.");
  }
  return result;
}

}    // namespace joda::results
