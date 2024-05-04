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
#include <string>
#include <thread>
#include <vector>
#include "../helper/helper.hpp"
#include "../logger/console_logger.hpp"
#include "backend/duration_count/duration_count.h"
#include "backend/helper/file_info.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/helper/system_resources.hpp"
#include "backend/helper/thread_pool.hpp"
#include "backend/helper/thread_pool_utils.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/pipelines/pipeline_steps/calc_count/calc_count.hpp"
#include "backend/pipelines/pipeline_steps/calc_intensity/calc_intensity.hpp"
#include "backend/pipelines/pipeline_steps/calc_intersection/calc_intersection.hpp"
#include "backend/results/results.hpp"
#include "backend/results/results_helper.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"
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
                   joda::helper::ImageFileContainer *imageFileContainer, const std::string &inputFolder,
                   const std::string &jobName, const ThreadingSettings &threadingSettings) :
    mInputFolder(inputFolder),
    mAnalyzeSettings(settings), mImageFileContainer(imageFileContainer), mThreadingSettings(threadingSettings),
    mJobName(jobName), mListOfChannelSettings(settings.channels.size()),
    mListOfVChannelSettings(settings.vChannels.size())
{
  try {
    mOutputFolder = prepareOutputFolder(inputFolder, jobName);
    //
    // Prepare settings
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
  joda::settings::Settings::storeSettings(mOutputFolder + separator + "settings_" + mJobName + ".json",
                                          mAnalyzeSettings);

  // Look for onnx models in the model folder
  mOnnxModels = onnx::OnnxParser::findOnnxFiles();

  // Look for images in the input folder
  mImageFileContainer->setWorkingDirectory(mInputFolder);
  mImageFileContainer->waitForFinished();
  mProgress.total.total = mImageFileContainer->getNrOfFiles();
  mProgress.image.total = mThreadingSettings.totalRuns;

  joda::results::WorkSheet alloverReport;
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
      futures.push_back(mGlobThreadPool.submit_task(
          [this, &images, &alloverReport, &image]() { analyzeImage(alloverReport, image); }));
      if(mStop) {
        break;
      }
    }
    futures.wait();

  } else {
    for(auto &image : images) {
      analyzeImage(alloverReport, image);
    }
  }

  auto timeStopped = std::chrono::high_resolution_clock::now();

  std::string resultsFile = mOutputFolder + separator + RESULTS_FOLDER_PATH + separator + "results_summary_" + mJobName;
  alloverReport.saveToFile(resultsFile, joda::results::WorkSheet::Meta{
                                            .swVersion    = Version::getVersion(),
                                            .buildTime    = Version::getBuildTime(),
                                            .jobName      = mJobName,
                                            .timeStarted  = mTimePipelineStarted,
                                            .timeFinished = std::chrono::system_clock::now(),
                                            .nrOfChannels = 1,
                                        });
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
void Pipeline::analyzeImage(joda::results::WorkSheet &alloverReport, const FileInfo &imagePath)
{
  std::string imageName       = helper::getFileNameFromPath(imagePath.getPath());
  std::string imageParentPath = helper::getFolderNameFromPath(imagePath.getPath());

  static const std::string separator(1, std::filesystem::path::preferred_separator);
  auto detailOutputFolder = mOutputFolder + separator + "images" + separator + imageName;

  std::filesystem::create_directories(detailOutputFolder);

  //
  // Execute for each tile
  //

  auto series   = mAnalyzeSettings.channels.begin()->meta.series;
  auto propsOut = joda::algo::ImageProcessor::loadChannelProperties(imagePath.getPath(), series);

  int64_t runs = 1;
  if(propsOut.props.imageSize > joda::algo::MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
    runs = propsOut.props.nrOfTiles / joda::algo::TILES_TO_LOAD_PER_RUN;
  }

  //
  // Iterate over each tile
  //
  joda::results::WorkSheet detailReport;
  std::mutex writeDetailReportMutex;

  int poolSize = mThreadingSettings.cores[ThreadingSettings::TILES];
  if(poolSize > 1) {
    auto futures = mGlobThreadPool.submit_sequence<unsigned int>(
        0, runs, [this, &detailReport, &imagePath, &detailOutputFolder, &propsOut](const unsigned int tileIdx) {
          analyzeTile(detailReport, imagePath, detailOutputFolder, tileIdx, propsOut);
        });
    futures.wait();
  } else {
    for(int tileIdx = 0; tileIdx < runs; tileIdx++) {
      analyzeTile(detailReport, imagePath, detailOutputFolder, tileIdx, propsOut);
      if(mStop) {
        break;
      }
    }
  }

  //
  // Write report
  //
  if(mState != State::ERROR_) {
    auto id = DurationCount::start("Write detail report");
    std::string fName =
        mOutputFolder + separator + RESULTS_FOLDER_PATH + separator + "results_image_" + imageName + "_" + mJobName;
    detailReport.saveToFile(fName, joda::results::WorkSheet::Meta{
                                       .swVersion    = Version::getVersion(),
                                       .buildTime    = Version::getBuildTime(),
                                       .jobName      = mJobName,
                                       .timeStarted  = mTimePipelineStarted,
                                       .timeFinished = std::chrono::system_clock::now(),
                                       .nrOfChannels = 1,
                                   });

    DurationCount::stop(id);

    id                = DurationCount::start("Append to overall report");
    auto nrOfChannels = joda::settings::Settings::getNrOfAllChannels(mAnalyzeSettings) + 1;
    joda::results::Helper::appendToAllOverReport(mAnalyzeSettings, alloverReport, detailReport, imageParentPath,
                                                 imageName, nrOfChannels);
    DurationCount::stop(id);
  }
  mProgress.total.finished++;
}

///
/// \brief      Analyze tile
/// \author     Joachim Danmayr
///
void Pipeline::analyzeTile(joda::results::WorkSheet &detailReports, FileInfo imagePath, std::string detailOutputFolder,
                           int tileIdx, const joda::algo::ChannelProperties &channelProperties)
{
  std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> detectionResults;

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
          [this, &detectionResults, &imagePath, &detailOutputFolder, &channelProperties, tileIdx,
           &referenceSpotChannels](unsigned int idx) {
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
        [this, &detectionResults, &imagePath, &detailOutputFolder, &channelProperties, tileIdx](unsigned int idx) {
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

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Execute intersection calculation
  //
  auto calcIntersection = [this, &imagePath, &detectionResults, &detailOutputFolder, &detailReports,
                           &channelProperties](int tileIdx, settings::VChannelIntersection intersect) {
    joda::pipeline::CalcIntersection intersectAlgo(intersect.intersection.intersectingChannels,
                                                   intersect.intersection.minIntersection);
    auto response = intersectAlgo.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
    detectionResults.emplace(intersect.meta.channelIdx, response);
    joda::results::Helper::setDetailReportHeader(mAnalyzeSettings, detailReports, intersect.meta.name,
                                                 intersect.meta.channelIdx);
    joda::results::Helper::appendToDetailReport(mAnalyzeSettings, detectionResults.at(intersect.meta.channelIdx),
                                                detailReports, detailOutputFolder, mJobName, intersect.meta.channelIdx,
                                                tileIdx, channelProperties.props, imagePath.getPath());
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
  auto calcVoronoi = [this, &imagePath, &detectionResults, &detailOutputFolder, &detailReports,
                      &channelProperties](int tileIdx, settings::VChannelVoronoi voronoi) {
    joda::pipeline::CalcVoronoi function(voronoi.meta.channelIdx, voronoi.voronoi.gridPointsChannelIdx,
                                         voronoi.voronoi.overlayMaskChannelIdx, voronoi.voronoi.maxVoronoiAreaRadius,
                                         voronoi.objectFilter.excludeAreasWithoutCenterOfMass,
                                         voronoi.objectFilter.excludeAreasAtEdges, voronoi.objectFilter.minParticleSize,
                                         voronoi.objectFilter.maxParticleSize);
    auto response = function.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);

    auto idx = voronoi.meta.channelIdx;

    detectionResults.emplace(idx, response);
    joda::results::Helper::setDetailReportHeader(mAnalyzeSettings, detailReports, voronoi.meta.name, idx);

    if(!voronoi.crossChannel.crossChannelIntensityChannels.empty()) {
      CalcIntensity intensity(idx, voronoi.crossChannel.crossChannelIntensityChannels);
      intensity.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
    }

    if(!voronoi.crossChannel.crossChannelCountChannels.empty()) {
      CalcCount counting(idx, voronoi.crossChannel.crossChannelCountChannels);
      counting.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
    }

    joda::results::Helper::appendToDetailReport(mAnalyzeSettings, detectionResults.at(idx), detailReports,
                                                detailOutputFolder, mJobName, idx, tileIdx, channelProperties.props,
                                                imagePath.getPath());
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
  auto writeStats = [this, &imagePath, &detectionResults, &detailOutputFolder, &detailReports,
                     &channelProperties](int tileIdx, settings::ChannelSettings channelSettings) {
    //
    // Measure intensity from ROI area of channel X in channel Y
    //
    if(!channelSettings.crossChannel.crossChannelCountChannels.empty()) {
      CalcCount counting(channelSettings.meta.channelIdx, channelSettings.crossChannel.crossChannelCountChannels);
      counting.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
    }

    //
    // Count ROIs from channel X in channel Y
    //
    if(!channelSettings.crossChannel.crossChannelIntensityChannels.empty()) {
      CalcIntensity intensity(channelSettings.meta.channelIdx,
                              channelSettings.crossChannel.crossChannelIntensityChannels);
      intensity.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
    }

    //
    // This is the last stage, write the detail settings
    //
    if(mState != State::ERROR_) {
      joda::results::Helper::setDetailReportHeader(mAnalyzeSettings, detailReports, channelSettings.meta.name,
                                                   channelSettings.meta.channelIdx);
    }
    if(detectionResults.contains(channelSettings.meta.channelIdx)) {
      joda::results::Helper::appendToDetailReport(
          mAnalyzeSettings, detectionResults.at(channelSettings.meta.channelIdx), detailReports, detailOutputFolder,
          mJobName, channelSettings.meta.channelIdx, tileIdx, channelProperties.props, imagePath.getPath());
    }
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

  tmr.stop();
  std::cout << "Post processing " << tmr.ms() << " ms.\n";
}

///
/// \brief      Analyze channel
/// \author     Joachim Danmayr
///
void Pipeline::analyszeChannel(std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> &detectionResults,
                               const joda::settings::ChannelSettings &channelSettings, FileInfo imagePath, int tileIdx,
                               const joda::algo::ChannelProperties &channelProperties)
{
  joda::settings::ChannelIndex channelIndex = channelSettings.meta.channelIdx;

  try {
    auto processingResult = joda::algo::ImageProcessor::executeAlgorithm(
        imagePath, channelSettings, tileIdx, mOnnxModels, &channelProperties, &detectionResults);
    // Add processing result to the detection result map
    std::lock_guard<std::mutex> lock(mAddToDetailReportMutex);
    detectionResults.emplace(channelIndex, processingResult);
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
  }
  mProgress.image.finished++;
}

///
/// \brief      Creates the output folder for the results and returns the path.
///             Outputfolder = <inputFolder>/results/<DATE-TIME>
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
/// \return     Outputfolder of the results
///
[[nodiscard]] auto Pipeline::prepareOutputFolder(const std::string &inputFolder, const std::string &jobName)
    -> std::string
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  auto nowString    = ::joda::helper::timeNowToString();
  auto outputFolder = inputFolder + separator + OUTPUT_FOLDER_PATH + separator + nowString + "_" + jobName;
  try {
    bool directoryExists = false;
    if(!std::filesystem::exists(outputFolder)) {
      directoryExists = std::filesystem::create_directories(outputFolder);
      if(!directoryExists) {
        setStateError("Can not create output folder!");
      }

      if(!std::filesystem::exists(outputFolder + separator + RESULTS_FOLDER_PATH)) {
        auto directoryExists = std::filesystem::create_directories(outputFolder + separator + RESULTS_FOLDER_PATH);
        if(!directoryExists) {
          joda::log::logError("Could not create results directory!");
        }
      }

      if(!std::filesystem::exists(outputFolder + separator + REPORT_EXPORT_FOLDER_PATH)) {
        auto directoryExists =
            std::filesystem::create_directories(outputFolder + separator + REPORT_EXPORT_FOLDER_PATH);
        if(!directoryExists) {
          joda::log::logError("Could not create report export directory!");
        }
      }

    } else {
      directoryExists = true;
    }
    return outputFolder;
  } catch(const std::exception &ex) {
    setStateError(ex.what());
  }
}

}    // namespace joda::pipeline
