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
#include "backend/helper/file_info.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/helper/system_resources.hpp"
#include "backend/helper/thread_pool.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/pipelines/pipeline_steps/calc_count/calc_count.hpp"
#include "backend/pipelines/pipeline_steps/calc_intensity/calc_intensity.hpp"
#include "backend/pipelines/pipeline_steps/calc_intersection/calc_intersection.hpp"
#include "backend/pipelines/reporting/reporting_heatmap.hpp"
#include "backend/pipelines/reporting/reporting_helper.hpp"
#include "backend/results/results_container.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"
#include "backend/settings/settings.hpp"
#include "pipeline_steps/calc_voronoi/calc_voronoi.hpp"
#include "processor/image_processor.hpp"
#include <opencv2/imgcodecs.hpp>

namespace joda::pipeline {

using namespace std;
using namespace std::filesystem;

Pipeline::Pipeline(const joda::settings::AnalyzeSettings &settings,
                   joda::helper::ImageFileContainer *imageFileContainer, const std::string &inputFolder,
                   const std::string &jobName, const ThreadingSettings &threadingSettings) :
    mInputFolder(inputFolder),
    mAnalyzeSettings(settings), mImageFileContainer(imageFileContainer), mThreadingSettings(threadingSettings),
    mJobName(jobName)
{
  try {
    mOutputFolder = prepareOutputFolder(inputFolder, jobName);
    mMainThread   = std::make_shared<std::thread>(&Pipeline::runJob, this);
    mState        = State::RUNNING;
  } catch(const std::exception &) {
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
  auto timeStarted = std::chrono::high_resolution_clock::now();
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

  int threadPoolImage = mThreadingSettings.cores[ThreadingSettings::IMAGES];
  BS::thread_pool imageThreadPool(threadPoolImage);
  auto idStart = DurationCount::start("analyze");

  std::map<std::string, joda::results::ReportingContainer> alloverReport;
  for(const auto &imagePath : mImageFileContainer->getFilesList()) {
    if(threadPoolImage > 1) {
      imageThreadPool.push_task([this, &alloverReport, imagePath] { analyzeImage(alloverReport, imagePath); });

      while(imageThreadPool.get_tasks_total() > (threadPoolImage + THREAD_POOL_BUFFER)) {
        std::this_thread::sleep_for(100us);
      }
    } else {
      analyzeImage(alloverReport, imagePath);
    }
    if(mStop) {
      break;
    }
  }

  imageThreadPool.wait_for_tasks();
  DurationCount::stop(idStart);
  auto timeStopped = std::chrono::high_resolution_clock::now();

  std::string resultsFile = mOutputFolder + separator + "results_summary_" + mJobName + ".xlsx";
  joda::results::ReportingContainer::flushReportToFile(
      mAnalyzeSettings, alloverReport, resultsFile,
      {.jobName = mJobName, .timeStarted = timeStarted, .timeFinished = timeStopped},
      joda::results::ReportingContainer::OutputFormat::HORIZONTAL, true);
  if(mAnalyzeSettings.experimentSettings.generateHeatmapForPlate) {
    auto wellOrder = mAnalyzeSettings.experimentSettings.generateHeatmapForWell
                         ? mAnalyzeSettings.experimentSettings.wellImageOrder
                         : std::vector<std::vector<int32_t>>();
    resultsFile    = mOutputFolder + separator + "heatmap_summary_" + mJobName + ".xlsx";
    joda::pipeline::reporting::Heatmap::createAllOverHeatMap(mAnalyzeSettings, alloverReport, mOutputFolder,
                                                             resultsFile, mJobName, wellOrder);
  }

  mState = State::FINISHED;

  while(!mStop) {
    sleep(1);
  }
}

///
/// \brief      Analyze image
/// \author     Joachim Danmayr
///
void Pipeline::analyzeImage(std::map<std::string, joda::results::ReportingContainer> &alloverReport,
                            const FileInfo &imagePath)
{
  int threadPoolTile = mThreadingSettings.cores[ThreadingSettings::TILES];
  BS::thread_pool tileThreadPool(threadPoolTile);

  std::string imageName       = helper::getFileNameFromPath(imagePath.getPath());
  std::string imageParentPath = helper::getFolderNameFromPath(imagePath.getPath());

  static const std::string separator(1, std::filesystem::path::preferred_separator);
  auto detailOutputFolder = mOutputFolder + separator + "images" + separator + imageName;

  std::filesystem::create_directories(detailOutputFolder);

  //
  // Execute for each tile
  //
  ImageProperties propsOut;
  if(imagePath.getDecoder() == FileInfo::Decoder::JPG) {
    propsOut = JpgLoader::getImageProperties(imagePath.getPath());
  } else if(imagePath.getDecoder() == FileInfo::Decoder::TIFF) {
    propsOut = TiffLoader::getImageProperties(imagePath.getPath(), 0);
  } else {
    if(!mAnalyzeSettings.channels.empty()) {
      auto series       = mAnalyzeSettings.channels.begin()->meta.series;
      auto [ome, props] = BioformatsLoader::getOmeInformation(imagePath.getPath(), series);
      propsOut          = props;
    }
  }
  int64_t runs = 1;
  if(propsOut.imageSize > joda::algo::MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
    runs = propsOut.nrOfTiles / joda::algo::TILES_TO_LOAD_PER_RUN;
  }

  //
  // Iterate over each tile
  //
  std::map<std::string, joda::results::ReportingContainer> detailReports;
  std::mutex writeDetailReportMutex;
  joda::results::ReportingContainer &detailReport = detailReports[""];

  for(uint32_t tileIdx = 0; tileIdx < runs; tileIdx++) {
    if(threadPoolTile > 1) {
      tileThreadPool.push_task(
          [this, &detailReport, &imagePath, &detailOutputFolder, &propsOut](int tileIdx) {
            analyzeTile(detailReport, imagePath, detailOutputFolder, tileIdx, propsOut);
          },
          tileIdx);

      while(tileThreadPool.get_tasks_total() > (threadPoolTile + THREAD_POOL_BUFFER)) {
        std::this_thread::sleep_for(100us);
      }
    } else {
      analyzeTile(detailReport, imagePath, detailOutputFolder, tileIdx, propsOut);
    }
    if(mStop) {
      break;
    }
  }
  tileThreadPool.wait_for_tasks();

  //
  // Write report
  //
  if(mState != State::ERROR_) {
    joda::results::ReportingContainer::flushReportToFile(
        mAnalyzeSettings, detailReports, detailOutputFolder + separator + "results_image_" + mJobName + ".xlsx",
        {.jobName = mJobName}, joda::results::ReportingContainer::OutputFormat::VERTICAL, false);

    if(mAnalyzeSettings.experimentSettings.generateHeatmapForImage) {
      joda::pipeline::reporting::Heatmap::createHeatMapForImage(
          mAnalyzeSettings, detailReport, propsOut.width, propsOut.height,
          detailOutputFolder + separator + "heatmap_image_" + mJobName + ".xlsx");
    }

    auto nrOfChannels = joda::settings::Settings::getNrOfAllChannels(mAnalyzeSettings) + 1;
    joda::pipeline::reporting::Helper::appendToAllOverReport(mAnalyzeSettings, alloverReport, detailReport,
                                                             imageParentPath, imageName, nrOfChannels);
  }
  mProgress.total.finished++;
}

///
/// \brief      Analyze tile
/// \author     Joachim Danmayr
///
void Pipeline::analyzeTile(joda::results::ReportingContainer &detailReports, FileInfo imagePath,
                           std::string detailOutputFolder, int tileIdx, const ImageProperties &imgProps)
{
  auto idChannels = DurationCount::start("channels");
  std::map<int32_t, joda::func::DetectionResponse> detectionResults;
  int threadPoolChannel = mThreadingSettings.cores[ThreadingSettings::CHANNELS];
  BS::thread_pool channelThreadPool(threadPoolChannel);

  //
  // Analyze the reference spots first
  //
  auto referenceSpotChannels = settings::Settings::getChannelsOfType(
      mAnalyzeSettings, joda::settings::ChannelSettingsMeta::Type::SPOT_REFERENCE);
  if(!referenceSpotChannels.empty()) {
    for(const auto *referenceSpot : referenceSpotChannels) {
      if(threadPoolChannel > 1) {
        channelThreadPool.push_task(
            [this, &detailReports, &detectionResults, &imagePath, &detailOutputFolder,
             &imgProps](const joda::settings::ChannelSettings *channelSettings, int tileIdx) {
              analyszeChannel(detectionResults, *channelSettings, imagePath, tileIdx);
            },
            referenceSpot, tileIdx);
        while(channelThreadPool.get_tasks_total() > (threadPoolChannel + THREAD_POOL_BUFFER)) {
          std::this_thread::sleep_for(10ms);
        }
      } else {
        analyszeChannel(detectionResults, *referenceSpot, imagePath, tileIdx);
      }
    }
    channelThreadPool.wait_for_tasks();
  }

  //
  // Iterate over each channel except reference spots
  //
  for(auto const &channelSettings : mAnalyzeSettings.channels) {
    if(channelSettings.meta.type != settings::ChannelSettingsMeta::Type::SPOT_REFERENCE) {
      if(threadPoolChannel > 1) {
        channelThreadPool.push_task(
            [this, &detectionResults, &imagePath, &detailOutputFolder, &imgProps, channelSettings = channelSettings](
                int tileIdx) { analyszeChannel(detectionResults, channelSettings, imagePath, tileIdx); },
            tileIdx);
        while(channelThreadPool.get_tasks_total() > (threadPoolChannel + THREAD_POOL_BUFFER)) {
          std::this_thread::sleep_for(10ms);
        }
      } else {
        analyszeChannel(detectionResults, channelSettings, imagePath, tileIdx);
      }
    }
    if(mStop) {
      break;
    }
  }
  channelThreadPool.wait_for_tasks();
  DurationCount::stop(idChannels);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Execute intersection calculation
  //
  auto idColoc = DurationCount::start("pipelinesteps");

  /* std::map<std::string, std::set<int32_t>> colocGroups;    // Groupname, Channels in the group
  std::map<std::string, float> minColocFactor;
  for(const auto &channel : mAnalyzeSettings.channels) {
    for(const auto &colocGroup : channel.getCrossChannelSettings().getColocGroups()) {
      colocGroups[colocGroup].emplace(channel.getChannelInfo().getChannelIndex());
      if(!minColocFactor.contains(colocGroup)) {
        minColocFactor[colocGroup] = channel.getCrossChannelSettings().getMinColocArea();
      } else {
        minColocFactor[colocGroup] =
            std::max(minColocFactor[colocGroup], channel.getCrossChannelSettings().getMinColocArea());
      }
    }
  }

  int colocIx = 0;
  for(const auto &[groupName, set] : colocGroups) {
    joda::pipeline::CalcIntersection intersect(set, minColocFactor[groupName]);
    auto response = intersect.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
    int idx       = settings::json::PipelineStepSettings::INTERSECTION_INDEX_OFFSET + colocIx;
    detectionResults.emplace(static_cast<int32_t>(idx), response);
    joda::pipeline::reporting::Helper::setDetailReportHeader(mAnalyzeSettings, detailReports, "Intersection", idx,
                                                             tempChannelIdx, set);
    joda::pipeline::reporting::Helper::appendToDetailReport(mAnalyzeSettings, detectionResults.at(idx), detailReports,
                                                            detailOutputFolder, mJobName, idx, tempChannelIdx, tileIdx,
                                                            imgProps);
    tempChannelIdx++;
    colocIx++;
    if(mStop) {
      break;
    }
  }
  */
  DurationCount::stop(idColoc);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Execute post processing pipeline steps
  //
  if(!mStop && mState != State::ERROR_) {
    auto idVoronoi = DurationCount::start("pipelinesteps");

    for(const auto &pipelineStep : mAnalyzeSettings.vChannels) {
      if(pipelineStep.$voronoi.has_value()) {
        const auto &voronoi = pipelineStep.$voronoi.value();
        joda::pipeline::CalcVoronoi function(voronoi.meta.channelIdx, voronoi.voronoi.gridPointsChannelIdx,
                                             voronoi.voronoi.overlayMaskChannelIdx,
                                             voronoi.voronoi.maxVoronoiAreaRadius);
        auto response = function.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);

        int idx = voronoi.meta.channelIdx;

        detectionResults.emplace(static_cast<int32_t>(idx), response);
        joda::pipeline::reporting::Helper::setDetailReportHeader(mAnalyzeSettings, detailReports, voronoi.meta.name,
                                                                 idx);

        if(!voronoi.crossChannel.crossChannelIntensityChannels.empty()) {
          CalcIntensity intensity(idx, voronoi.crossChannel.crossChannelIntensityChannels);
          intensity.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
        }

        if(!voronoi.crossChannel.crossChannelCoutChannels.empty()) {
          CalcCount counting(idx, voronoi.crossChannel.crossChannelCoutChannels);
          counting.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
        }

        joda::pipeline::reporting::Helper::appendToDetailReport(mAnalyzeSettings, detectionResults.at(idx),
                                                                detailReports, detailOutputFolder, mJobName, idx,
                                                                tileIdx, imgProps);
      }
      if(mStop) {
        break;
      }
    }
    DurationCount::stop(idVoronoi);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(!mStop && mState != State::ERROR_) {
    for(const auto &channelSettings : mAnalyzeSettings.channels) {
      //
      // Measure intensity from ROI area of channel X in channel Y
      //
      if(!channelSettings.crossChannel.crossChannelCoutChannels.empty()) {
        CalcCount counting(channelSettings.meta.channelIdx, channelSettings.crossChannel.crossChannelCoutChannels);
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
        joda::pipeline::reporting::Helper::setDetailReportHeader(
            mAnalyzeSettings, detailReports, channelSettings.meta.name, channelSettings.meta.channelIdx);
      }
      if(detectionResults.contains(channelSettings.meta.channelIdx)) {
        joda::pipeline::reporting::Helper::appendToDetailReport(
            mAnalyzeSettings, detectionResults.at(channelSettings.meta.channelIdx), detailReports, detailOutputFolder,
            mJobName, channelSettings.meta.channelIdx, tileIdx, imgProps);
      }
    }
  }
}

///
/// \brief      Analyze channel
/// \author     Joachim Danmayr
///
void Pipeline::analyszeChannel(std::map<int32_t, joda::func::DetectionResponse> &detectionResults,
                               const joda::settings::ChannelSettings &channelSettings, FileInfo imagePath, int tileIdx)
{
  int32_t channelIndex = channelSettings.meta.channelIdx;

  try {
    auto processingResult = joda::algo::ImageProcessor::executeAlgorithm(imagePath, channelSettings, tileIdx,
                                                                         mOnnxModels, &detectionResults);
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
  auto outputFolder = inputFolder + separator + RESULTS_PATH_NAME + separator + nowString + "_" + jobName;
  try {
    bool directoryExists = false;
    if(!std::filesystem::exists(outputFolder)) {
      directoryExists = std::filesystem::create_directories(outputFolder);
      if(!directoryExists) {
        setStateError("Can not create output folder!");
      }

      if(!std::filesystem::exists(outputFolder + separator + "heatmaps")) {
        auto directoryExists = std::filesystem::create_directories(outputFolder + separator + "heatmaps");
        if(!directoryExists) {
          joda::log::logError("Could not create heatmap directory!");
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
