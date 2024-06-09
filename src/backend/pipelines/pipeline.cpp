///
/// \file      pipeline.cpp
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

#include "pipeline.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include "../helper/helper.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_info.hpp"
#include "backend/helper/file_info_images.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/helper/system_resources.hpp"
#include "backend/helper/thread_pool.hpp"
#include "backend/helper/thread_pool_utils.hpp"
#include "backend/helper/uuid.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/reader/bioformats/bioformats_loader.hpp"
#include "backend/image_processing/reader/image_reader.hpp"
#include "backend/pipelines/pipeline_steps/calc_count/calc_count.hpp"
#include "backend/pipelines/pipeline_steps/calc_intensity/calc_intensity.hpp"
#include "backend/pipelines/pipeline_steps/calc_intersection/calc_intersection.hpp"
#include "backend/results/results.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"
#include "backend/settings/experiment_settings.hpp"
#include "backend/settings/settings.hpp"
#include "backend/settings/vchannel/vchannel_settings.hpp"
#include "backend/settings/vchannel/vchannel_voronoi_settings.hpp"
#include "pipeline_steps/calc_voronoi/calc_voronoi.hpp"
#include "processor/image_processor.hpp"
#include <opencv2/imgcodecs.hpp>
#include "version.h"

namespace joda::pipeline {

using namespace std;
using namespace std::filesystem;

BS::thread_pool mGlobThreadPool{10};

Pipeline::Pipeline(const joda::settings::AnalyzeSettings &settings,
                   joda::helper::fs::DirectoryWatcher<helper::fs::FileInfoImages> *imageFileContainer,
                   const std::filesystem::path &inputFolder, const std::string &analyzeName,
                   const ThreadingSettings &threadingSettings) :
    mInputFolder(inputFolder),
    mResults(std::filesystem::path(inputFolder),
             joda::results::ExperimentSetting{.runId              = helper::generate_uuid(),
                                              .analyzeName        = analyzeName,
                                              .scientistName      = "",
                                              .imageFileNameRegex = settings.experimentSettings.filenameRegex,
                                              .groupBy            = settings.experimentSettings.groupBy,
                                              .plateIdx           = 1,
                                              .plateRowNr         = 16,
                                              .plateColNr         = 16},
             settings),
    mAnalyzeSettings(settings), mImageFileContainer(imageFileContainer), mThreadingSettings(threadingSettings),
    mJobName(analyzeName), mListOfChannelSettings(settings.channels.size()),
    mListOfVChannelSettings(settings.vChannels.size())
{
  try {
    //
    //  Prepare settings
    //
    int idx = 0;
    for(const auto &ch : settings.channels) {
      mListOfChannelSettings[idx] = &ch;
      idx++;
    }
    idx = 0;
    for(const auto &ch : settings.vChannels) {
      mListOfVChannelSettings[idx] = &ch;
      idx++;
    }
    mGlobThreadPool.reset(threadingSettings.coresUsed);

    mMainThread = std::make_shared<std::thread>(&Pipeline::runJob, this);
    mState      = State::RUNNING;
  } catch(const std::exception &ex) {
    joda::log::logError("Could not start: " + std::string(ex.what()));
    mState = State::FINISHED;
    return;
  }
}

///
/// \brief      Runs the pipeline.
///             Iterates over all found images and executes
///             the pipeline for each image by calling the
///             pure virtual function >execute< which must
///             be defined in one of the derived classes.
/// \author     Joachim Danmayr
/// \return
///
void Pipeline::runJob()
{
  BS::timer tmr;
  tmr.start();

  DurationCount::resetStats();
  mTimePipelineStarted = std::chrono::high_resolution_clock::now();
  // Store configuration
  static const std::string separator(1, std::filesystem::path::preferred_separator);
  joda::settings::Settings::storeSettings((getOutputFolder() / ("settings_" + mJobName + ".json")).string(),
                                          mAnalyzeSettings);

  // Prepare database
  mResults.appendChannelsToDetailReport(mAnalyzeSettings);

  // Look for onnx models in the model folder
  mOnnxModels = onnx::OnnxParser::findOnnxFiles();

  // Look for images in the input folder
  mImageFileContainer->setWorkingDirectory(mInputFolder);
  mImageFileContainer->waitForFinished();
  mProgress.total.total = mImageFileContainer->getNrOfFiles();
  mProgress.image.total = mThreadingSettings.totalRuns;

  auto images = mImageFileContainer->getFilesList();

  tmr.stop();
  std::cout << "Preparing " << tmr.ms() << " ms.\n";

  int poolSize = mThreadingSettings.cores[ThreadingSettings::IMAGES];

  joda::log::logTrace("Image pool size >" + std::to_string(mThreadingSettings.cores[ThreadingSettings::IMAGES]) + "<" +
                      ">" + std::to_string(mThreadingSettings.cores[ThreadingSettings::TILES]) + "<>" +
                      std::to_string(mThreadingSettings.cores[ThreadingSettings::CHANNELS]) + "<.");

  if(poolSize > 1) {
    BS::multi_future<void> futures;
    BS::multi_future<void>::iterator iter;

    for(auto &image : images) {
      futures.push_back(mGlobThreadPool.submit_task([this, &images, &image]() { analyzeImage(image); }));
      if(mStop) {
        break;
      }
    }
    futures.wait();

  } else {
    for(auto &image : images) {
      analyzeImage(image);
    }
  }

  auto timeStopped = std::chrono::high_resolution_clock::now();

  // Analyze finished

  mState = State::FINISHED;
  DurationCount::printStats(images.size());

  while(!mStop) {
    sleep(1);
  }
}

void Pipeline::stopJob()
{
  if(mState != State::FINISHED && mState != State::ERROR_) {
    mState = State::STOPPING;
  }
  mStop = true;
  mGlobThreadPool.purge();
}

///
/// \brief      Analyze image
/// \author     Joachim Danmayr
///
void Pipeline::analyzeImage(const helper::fs::FileInfoImages &imagePath)
{
  //
  // Execute for each tile
  //
  auto series   = mAnalyzeSettings.channels.begin()->meta.series;
  auto propsOut = joda::pipeline::ImageProcessor::loadChannelProperties(imagePath, series);

  // Make entry in database
  mResults.appendImageToDetailReport(propsOut.props, imagePath.getFilePath());

  int64_t runs = 1;
  if(propsOut.props.imageSize > joda::pipeline::MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
    runs = propsOut.props.nrOfTiles / joda::pipeline::TILES_TO_LOAD_PER_RUN;
  }

  //
  // Iterate over each tile
  //
  int poolSize = mThreadingSettings.cores[ThreadingSettings::TILES];
  if(poolSize > 1) {
    auto futures = mGlobThreadPool.submit_sequence<unsigned int>(
        0, runs,
        [this, &imagePath, &propsOut](const unsigned int tileIdx) { analyzeTile(imagePath, tileIdx, propsOut); });
    futures.wait();
  } else {
    for(int tileIdx = 0; tileIdx < runs; tileIdx++) {
      analyzeTile(imagePath, tileIdx, propsOut);
      if(mStop) {
        break;
      }
    }
  }

  //
  // Write report
  //
  // Write to all over report if needed
  mProgress.total.finished++;
}

///
/// \brief      Analyze tile
/// \author     Joachim Danmayr
///
void Pipeline::analyzeTile(helper::fs::FileInfoImages imagePath, int tileIdx,
                           const joda::pipeline::ChannelProperties &channelProperties)
{
  std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> detectionResults;

  auto poolSize = static_cast<BS::concurrency_t>(mThreadingSettings.cores[ThreadingSettings::CHANNELS]);

  //
  // Analyze the reference spots first
  //
  auto referenceSpotChannels = settings::Settings::getChannelsOfType(
      mAnalyzeSettings, joda::settings::ChannelSettingsMeta::Type::SPOT_REFERENCE);
  if(poolSize > 1) {
    if(!referenceSpotChannels.empty()) {
      auto futures = mGlobThreadPool.submit_sequence<unsigned int>(
          0, referenceSpotChannels.size(),
          [this, &detectionResults, &imagePath, &channelProperties, tileIdx, &referenceSpotChannels](unsigned int idx) {
            analyszeChannel(detectionResults, *referenceSpotChannels.at(idx), imagePath, tileIdx, channelProperties);
          });
      futures.wait();
    }
  } else {
    for(auto const &channel : referenceSpotChannels) {
      analyszeChannel(detectionResults, *channel, imagePath, tileIdx, channelProperties);
      if(mStop) {
        break;
      }
    }
  }

  //
  // Iterate over each channel except reference spots
  //
  if(poolSize > 1) {
    auto futures = mGlobThreadPool.submit_sequence<unsigned int>(
        0, mListOfChannelSettings.size(),
        [this, &detectionResults, &imagePath, &channelProperties, tileIdx](unsigned int idx) {
          if(this->mListOfChannelSettings.at(idx)->meta.type !=
             joda::settings::ChannelSettingsMeta::Type::SPOT_REFERENCE) {
            BS::timer tmr;
            tmr.start();
            analyszeChannel(detectionResults, *this->mListOfChannelSettings.at(idx), imagePath, tileIdx,
                            channelProperties);
            tmr.stop();
            std::cout << "The elapsed time >" << imagePath.getFilename() << "< >" << std::to_string(idx) << "< was "
                      << tmr.ms() << " ms.\n";
          }
        });
    std::cout << "Wait for channel\n";

    BS::timer tmr;
    tmr.start();
    futures.wait();
    tmr.stop();
    std::cout << "Wait for pool >" << imagePath.getFilename() << "< " << tmr.ms() << " ms.\n";
  } else {
    for(auto const &channel : mListOfChannelSettings) {
      if(channel->meta.type != joda::settings::ChannelSettingsMeta::Type::SPOT_REFERENCE) {
        analyszeChannel(detectionResults, *channel, imagePath, tileIdx, channelProperties);
        if(mStop) {
          break;
        }
      }
    }
  }

  BS::timer tmr;
  tmr.start();

  auto [appender, connection] = mResults.prepareDetailReportAdding();
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Execute intersection calculation
  //
  auto calcIntersection = [this, appender = appender, &imagePath, &detectionResults,
                           &channelProperties](int tileIdx, settings::VChannelIntersection intersect) {
    joda::pipeline::CalcIntersection intersectAlgo(
        intersect.meta.channelIdx, intersect.intersection.intersectingChannels, intersect.intersection.minIntersection);
    auto response = intersectAlgo.execute(mAnalyzeSettings, detectionResults);
    detectionResults.emplace(intersect.meta.channelIdx, std::move(response));

    auto id = DurationCount::start("Append to detail report intersect");

    mResults.appendToDetailReport(appender, detectionResults.at(intersect.meta.channelIdx), intersect.meta, tileIdx,
                                  channelProperties.props, imagePath.getFilePath());
    DurationCount::stop(id);
  };

  if(poolSize > 1) {
    auto futures = mGlobThreadPool.submit_sequence<unsigned int>(
        0, mListOfVChannelSettings.size(), [this, &calcIntersection, tileIdx](unsigned int idx) {
          auto *val = this->mListOfVChannelSettings.at(idx);
          if(val->$intersection.has_value()) {
            calcIntersection(tileIdx, val->$intersection.value());
          }
        });
    futures.wait();
  } else {
    for(const auto &val : mListOfVChannelSettings) {
      if(val->$intersection.has_value()) {
        calcIntersection(tileIdx, val->$intersection.value());
      }
      if(mStop) {
        break;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Execute post processing pipeline steps
  //
  auto calcVoronoi = [this, appender = appender, &imagePath, &detectionResults,
                      &channelProperties](int tileIdx, settings::VChannelVoronoi voronoi) {
    joda::pipeline::CalcVoronoi function(voronoi.meta.channelIdx, voronoi.voronoi.gridPointsChannelIdx,
                                         voronoi.voronoi.overlayMaskChannelIdx, voronoi.voronoi.maxVoronoiAreaRadius,
                                         voronoi.objectFilter.excludeAreasWithoutCenterOfMass,
                                         voronoi.objectFilter.excludeAreasAtEdges, voronoi.objectFilter.minParticleSize,
                                         voronoi.objectFilter.maxParticleSize);
    auto response = function.execute(mAnalyzeSettings, detectionResults);

    auto idx = voronoi.meta.channelIdx;

    detectionResults.emplace(idx, std::move(response));

    if(!voronoi.crossChannel.crossChannelIntensityChannels.empty()) {
      CalcIntensity intensity(idx, voronoi.crossChannel.crossChannelIntensityChannels);
      intensity.execute(mAnalyzeSettings, detectionResults);
    }

    if(!voronoi.crossChannel.crossChannelCountChannels.empty()) {
      CalcCount counting(idx, voronoi.crossChannel.crossChannelCountChannels);
      counting.execute(mAnalyzeSettings, detectionResults);
    }
    auto id = DurationCount::start("Append to detail report vorono");

    mResults.appendToDetailReport(appender, detectionResults.at(voronoi.meta.channelIdx), voronoi.meta, tileIdx,
                                  channelProperties.props, imagePath.getFilePath());
    DurationCount::stop(id);
  };

  if(!mStop && mState != State::ERROR_) {
    for(const auto &pipelineStep : mAnalyzeSettings.vChannels) {
      if(pipelineStep.$voronoi.has_value()) {
        calcVoronoi(tileIdx, pipelineStep.$voronoi.value());
      }
      if(mStop) {
        break;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Channel stats
  //
  auto writeStats = [this, appender = appender, &imagePath, &detectionResults,
                     &channelProperties](int tileIdx, settings::ChannelSettings channelSettings) {
    //
    // Measure intensity from ROI area of channel X in channel Y
    //
    if(!channelSettings.crossChannel.crossChannelCountChannels.empty()) {
      CalcCount counting(channelSettings.meta.channelIdx, channelSettings.crossChannel.crossChannelCountChannels);
      counting.execute(mAnalyzeSettings, detectionResults);
    }

    //
    // Count ROIs from channel X in channel Y
    //
    if(!channelSettings.crossChannel.crossChannelIntensityChannels.empty()) {
      CalcIntensity intensity(channelSettings.meta.channelIdx,
                              channelSettings.crossChannel.crossChannelIntensityChannels);
      intensity.execute(mAnalyzeSettings, detectionResults);
    }

    auto id = DurationCount::start("Append to detail report stats");

    if(detectionResults.contains(channelSettings.meta.channelIdx)) {
      mResults.appendToDetailReport(appender, detectionResults.at(channelSettings.meta.channelIdx),
                                    channelSettings.meta, tileIdx, channelProperties.props, imagePath.getFilePath());
    }
    DurationCount::stop(id);
  };

  if(poolSize > 1) {
    auto futures = mGlobThreadPool.submit_sequence<unsigned int>(
        0, mListOfChannelSettings.size(),
        [this, &writeStats, tileIdx](unsigned int idx) { writeStats(tileIdx, *this->mListOfChannelSettings.at(idx)); });

    futures.wait();
  } else {
    for(const auto &channel : mListOfChannelSettings) {
      writeStats(tileIdx, *channel);
      if(mStop) {
        break;
      }
    }
  }
  mResults.writePredatedData(appender, connection);

  tmr.stop();
  std::cout << "Post processing " << tmr.ms() << " ms.\n";
}

///
/// \brief      Analyze channel
/// \author     Joachim Danmayr
///
void Pipeline::analyszeChannel(
    std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> &detectionResults,
    const joda::settings::ChannelSettings &channelSettings, helper::fs::FileInfoImages imagePath, int tileIdx,
    const ChannelProperties &channelProperties)
{
  joda::settings::ChannelIndex channelIndex = channelSettings.meta.channelIdx;

  try {
    auto processingResult = ImageProcessor::executeAlgorithm(imagePath, channelSettings, tileIdx, mOnnxModels,
                                                             &channelProperties, &detectionResults);
    // Add processing result to the detection result map
    std::lock_guard<std::mutex> lock(mAddToDetailReportMutex);
    detectionResults.emplace(channelIndex, std::move(processingResult));
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
  }
  mProgress.image.finished++;
}

}    // namespace joda::pipeline
