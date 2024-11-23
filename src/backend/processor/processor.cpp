///
/// \file      processor.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "processor.hpp"
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/enums/enums_grouping.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/threading/threading.hpp"
#include "backend/helper/threadpool/thread_pool.hpp"
#include "backend/helper/threadpool/thread_pool_utils.hpp"
#include "backend/processor/context/plate_context.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/processor/dependency_graph.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/settings.hpp"

namespace joda::processor {

BS::thread_pool mGlobThreadPool{10};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Processor::Processor()
{
}

void Processor::stop()
{
  mProgress.setStateStopping();
  mGlobThreadPool.purge();
}

void Processor::execute(const joda::settings::AnalyzeSettings &program, const std::string &jobName,
                        const joda::thread::ThreadingSettings &threadingSettings, imagesList_t &allImages)
{
  try {
    DurationCount::resetStats();
    // Prepare thread pool
    mGlobThreadPool.reset(threadingSettings.coresUsed);
    int poolSizeImages   = threadingSettings.cores.at(joda::thread::ThreadingSettings::IMAGES);
    int poolSizeChannels = threadingSettings.cores.at(joda::thread::ThreadingSettings::CHANNELS);
    int poolSizeTiles    = threadingSettings.cores.at(joda::thread::ThreadingSettings::TILES);

    // Resolve dependencies
    auto pipelineOrder = joda::processor::DependencyGraph::calcGraph(program);

    // Prepare the context
    GlobalContext globalContext;
    auto jobId = initializeGlobalContext(program, jobName, globalContext);

    // Looking for images in all folders
    listImages(program, allImages);

    //
    // Iterate over each plate and analyze the images
    //
    auto &db = globalContext.database;
    for(const auto &plate : program.projectSettings.plates) {
      BS::multi_future<void> imageFutures;
      PlateContext plateContext{.plateId = plate.plateId};
      const auto &images = allImages.getFilesListAt(plate.plateId);

      mProgress.setRunningPreparingPipeline();
      auto imagesToProcess = db.prepareImages(plate.plateId, plate.groupBy, plate.filenameRegex, images, mGlobThreadPool);
      mProgress.setStateRunning();

      //
      // Iterate over each image
      //
      for(const auto &actImage : imagesToProcess) {
        auto analyzeImage = [this, &program, &globalContext, &plateContext, &pipelineOrder, &db, &poolSizeTiles, &poolSizeChannels, &actImage]() {
          auto const [imagePath, omeInfo, imageId] = actImage;
          PipelineInitializer imageLoader(program.imageSetup);
          ImageContext imageContext{.imageLoader = imageLoader, .imagePath = imagePath, .imageMeta = omeInfo, .imageId = imageId};
          imageLoader.init(imageContext);

          //
          // Start the iteration over planes
          //
          auto [tilesX, tilesY] = imageLoader.getNrOfTilesToProcess();
          auto nrtStack         = imageLoader.getNrOfTStacksToProcess();
          auto nrzSTack         = imageLoader.getNrOfZStacksToProcess();
          auto nrChannels       = omeInfo.getNrOfChannels();

          mProgress.setTotalNrOfTiles(mProgress.totalImages() * tilesX * tilesY);
          BS::multi_future<void> tilesFutures;

          for(int tileX = 0; tileX < tilesX; tileX++) {
            if(mProgress.isStopping()) {
              break;
            }
            for(int tileY = 0; tileY < tilesY; tileY++) {
              if(mProgress.isStopping()) {
                break;
              }

              auto analyzeTile = [this, &program, &globalContext, &plateContext, &pipelineOrder, &db, imagePath = imagePath, nrtStack, nrzSTack,
                                  nrChannels, &imageContext, &imageLoader, tileX, tileY, &poolSizeChannels]() {
                // Start of the image specific function
                for(int tStack = 0; tStack < nrtStack; tStack++) {
                  if(mProgress.isStopping()) {
                    break;
                  }
                  for(int zStack = 0; zStack < nrzSTack; zStack++) {
                    if(mProgress.isStopping()) {
                      break;
                    }
                    IterationContext iterationContext;
                    // Execute pipelines of this iteration

                    // Start with the highest prio pipelines down to the lowest prio
                    for(const auto &[order, pipelines] : pipelineOrder) {
                      auto executePipelineOrder = [this, &program, &globalContext, &plateContext, &pipelineOrder, &db, imagePath, nrtStack, nrzSTack,
                                                   &imageContext, &imageLoader, tileX, tileY, nrChannels, pipelines = pipelines, &iterationContext,
                                                   tStack, zStack, &poolSizeChannels]() {
                        // These are pipelines in onw prio step -> Can be parallelized
                        BS::multi_future<void> pipelinesFutures;
                        for(const auto &pipelineToExecute : pipelines) {
                          if(mProgress.isStopping()) {
                            break;
                          }
                          auto executePipeline = [pipeline = pipelineToExecute, this, &program, &globalContext, &plateContext, &pipelineOrder, &db,
                                                  imagePath, nrtStack, nrzSTack, &imageContext, &imageLoader, tileX, tileY, nrChannels,
                                                  pipelines = pipelines, &iterationContext, tStack, zStack]() {
                            //
                            // Load the image imagePlane
                            //
                            ProcessContext context{globalContext, plateContext, imageContext, iterationContext};
                            imageLoader.initPipeline(pipeline->pipelineSetup, {tileX, tileY},
                                                     {.tStack = tStack, .zStack = zStack, .cStack = pipeline->pipelineSetup.cStackIndex}, context);
                            auto planeId = context.getActImage().getId().imagePlane;
                            try {
                              if(pipeline->pipelineSetup.cStackIndex >= 0 && pipeline->pipelineSetup.cStackIndex < nrChannels) {
                                db.insertImagePlane(imageContext.imageId, planeId,
                                                    imageContext.imageMeta.getChannelInfos()
                                                        .at(pipeline->pipelineSetup.cStackIndex)
                                                        .planes.at(planeId.tStack)
                                                        .at(planeId.zStack));
                              }
                            } catch(const std::exception &ex) {
                              std::cout << "Insert Plane: " << ex.what() << std::endl;
                            }

                            //
                            // Execute the pipeline
                            //
                            for(const auto &step : pipeline->pipelineSteps) {
                              if(mProgress.isStopping()) {
                                break;
                              }
                              // Execute a pipeline step
                              step(context, context.getActImage().image, context.getActObjects());
                            }
                          };

                          if(poolSizeChannels > 1) {
                            pipelinesFutures.push_back(mGlobThreadPool.submit_task(executePipeline));
                          } else {
                            executePipeline();
                          }
                        }
                        if(poolSizeChannels > 1) {
                          pipelinesFutures.wait();
                        }
                      };
                      executePipelineOrder();
                    }

                    // Iteration for all tiles finished
                    auto id = DurationCount::start("Insert");
                    try {
                      db.insertObjects(imageContext, iterationContext.getObjects());
                    } catch(const std::exception &ex) {
                      std::cout << "Insert Obj: " << ex.what() << std::endl;
                    }
                    DurationCount::stop(id);
                  }
                }
                // Tile finished
                mProgress.incProcessedTiles();
              };

              if(poolSizeTiles > 1) {
                tilesFutures.push_back(mGlobThreadPool.submit_task(analyzeTile));
              } else {
                analyzeTile();
              }
            }
          }
          if(poolSizeTiles > 1) {
            tilesFutures.wait();
          }

          // Image finished
          db.setImageProcessed(imageContext.imageId);
          mProgress.incProcessedImages();
        };

        if(poolSizeImages > 1) {
          imageFutures.push_back(mGlobThreadPool.submit_task(analyzeImage));
        } else {
          analyzeImage();
        }
      }
      if(poolSizeImages > 1) {
        imageFutures.wait();
      }
    }

    // Done
    db.finishJob(jobId);
    mProgress.setStateFinished();
    DurationCount::printStats(allImages.getNrOfFiles(), mJobInformation.ouputFolder);

  } catch(const std::exception &ex) {
    mProgress.setStateError(mJobInformation, ex.what());
  }
}

std::string Processor::initializeGlobalContext(const joda::settings::AnalyzeSettings &program, const std::string &jobName,
                                               GlobalContext &globalContext)
{
  auto now = std::chrono::system_clock::now();
  mProgress.reset();
  globalContext.resultsOutputFolder =
      std::filesystem::path(program.projectSettings.workingDirectory) / "imagec" / (joda::helper::timepointToIsoString(now) + "_" + jobName);

  std::filesystem::create_directories(globalContext.resultsOutputFolder);

  settings::Settings::storeSettings((globalContext.resultsOutputFolder / ("settings" + joda::fs::EXT_PROJECT)), program);

  mJobInformation.resultsFilePath  = globalContext.resultsOutputFolder / ("results" + joda::fs::EXT_DATABASE);
  mJobInformation.ouputFolder      = globalContext.resultsOutputFolder;
  mJobInformation.jobName          = jobName;
  mJobInformation.timestampStarted = now;
  auto &db                         = globalContext.database;
  db.openDatabase(globalContext.resultsOutputFolder / ("results" + joda::fs::EXT_DATABASE));
  return db.startJob(program, jobName);
}

void Processor::listImages(const joda::settings::AnalyzeSettings &program, imagesList_t &allImages)
{
  mProgress.setStateLookingForImages();
  for(const auto &plate : program.projectSettings.plates) {
    allImages.setWorkingDirectory(plate.plateId, plate.imageFolder);
  }
  allImages.waitForFinished();
  mProgress.setTotalNrOfImages(allImages.getNrOfFiles());
  mProgress.setRunningPreparingPipeline();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Processor::generatePreview(const PreviewSettings &previewSettings, const settings::ProjectImageSetup &imageSetup,
                                const settings::AnalyzeSettings &program, const settings::Pipeline &pipelineStart,
                                const std::filesystem::path &imagePath, int32_t tStack, int32_t zStack, int32_t tileX, int32_t tileY,
                                bool generateThumb, const ome::OmeInfo &ome, const settings::ObjectInputClusters &clustersClassesToShow)
    -> std::tuple<cv::Mat, cv::Mat, cv::Mat, std::map<settings::ClassificatorSetting, PreviewReturn>>
{
  auto ii = DurationCount::start("Generate preview");

  //
  //  Resolve dependencies
  //  We only want to execute those pipelines which are needed for the preview
  //
  auto pipelineOrder = joda::processor::DependencyGraph::calcGraph(program, &pipelineStart);

  //
  // Generate preview in a thread
  //
  cv::Mat thumb;
  auto thumbThread = std::thread([&]() {
    if(generateThumb) {
      auto i = DurationCount::start("Generate thumb");
      thumb  = joda::image::reader::ImageReader::loadThumbnail(
          imagePath.string(), joda::image::reader::ImageReader::Plane{.z = zStack, .c = pipelineStart.pipelineSetup.cStackIndex, .t = tStack}, 0,
          ome);
      DurationCount::stop(i);
    }
  });

  //
  // Get image
  //
  GlobalContext globalContext;
  PlateContext plateContext{.plateId = 0};
  joda::grp::FileGrouper grouper(enums::GroupBy::OFF, "");
  PipelineInitializer imageLoader(imageSetup);
  ImageContext imageContext{.imageLoader = imageLoader, .imagePath = imagePath, .imageMeta = ome, .imageId = 1};
  imageLoader.init(imageContext);

  IterationContext iterationContext;

  int32_t totalRuns = 0;
  for(const auto &[order, pipelines] : pipelineOrder) {
    for(const auto &pipeline : pipelines) {
      totalRuns++;
    }
  }

  int executedSteps = 0;
  int colorIdx      = 0;
  for(const auto &[order, pipelines] : pipelineOrder) {
    for(const auto &pipeline : pipelines) {
      executedSteps++;
      //
      // Load the image imagePlane
      //
      ProcessContext context{globalContext, plateContext, imageContext, iterationContext};
      imageLoader.initPipeline(pipeline->pipelineSetup, {tileX, tileY},
                               {.tStack = tStack, .zStack = zStack, .cStack = pipeline->pipelineSetup.cStackIndex}, context);
      auto planeId = context.getActImage().getId().imagePlane;

      //
      // Execute the pipeline
      //
      for(const auto &step : pipeline->pipelineSteps) {
        if(step.$saveImage.has_value()) {
          // For preview do not execute image saver
          continue;
        }
        step(context, context.getActImage().image, context.getActObjects());
      }

      //
      // The last step is the wanted pipeline
      //
      if(executedSteps >= totalRuns) {
        //
        // Count elements
        //
        std::map<settings::ClassificatorSetting, PreviewReturn> foundObjects;
        // auto cluster = pipelineStart.getOutputCluster();
        {
          for(auto const &[cluster, objects] : context.getActObjects()) {
            for(const auto &roi : *objects) {
              settings::ClassificatorSetting key{static_cast<enums::ClusterIdIn>(cluster), static_cast<enums::ClassIdIn>(roi.getClassId())};
              if(!foundObjects.contains(key)) {
                foundObjects[key].count       = 0;
                foundObjects[key].color       = "#000000";
                foundObjects[key].wantedColor = settings::IMAGE_SAVER_COLORS[colorIdx % settings::IMAGE_SAVER_COLORS.size()];
                colorIdx++;
              }
              foundObjects[key].count++;
            }
          }
        }

        //
        // Generate preview image
        //
        joda::settings::ImageSaverSettings saverSettings;
        saverSettings.clustersIn.clear();
        for(const auto &clusterClass : clustersClassesToShow) {
          auto cluster = clusterClass.clusterId;
          std::cout << "Clutseras and classes to show" << std::endl;
          auto classs = clusterClass.classId;
          if(cluster == enums::ClusterIdIn::$) {
            cluster = static_cast<enums::ClusterIdIn>(pipelineStart.pipelineSetup.defaultClusterId);
          }

          if(classs == enums::ClassIdIn::$) {
            classs = static_cast<enums::ClassIdIn>(pipelineStart.pipelineSetup.defaultClassId);
          }

          auto key = settings::ClassificatorSetting{cluster, classs};
          if(foundObjects.contains(key)) {
            // Objects which are selected should be painted in color in the legend, not selected are black
            foundObjects[key].color = foundObjects.at(key).wantedColor;

            saverSettings.clustersIn.emplace_back(settings::ImageSaverSettings::SaveCluster{.inputCluster     = {cluster, classs},
                                                                                            .color            = foundObjects.at(key).wantedColor,
                                                                                            .style            = previewSettings.style,
                                                                                            .paintBoundingBox = false});
          }
        }

        saverSettings.canvas     = settings::ImageSaverSettings::Canvas::IMAGE_$;
        saverSettings.planesIn   = enums::ImageId{.zProjection = enums::ZProjection::$};
        saverSettings.outputSlot = settings::ImageSaverSettings::Output::IMAGE_$;
        auto step                = settings::PipelineStep{.$saveImage = saverSettings};
        auto saver               = joda::settings::PipelineFactory<joda::cmd::Command>::generate(step);
        saver->execute(context, context.getActImage().image, context.getActObjects());

        thumbThread.join();
        DurationCount::stop(ii);
        return {context.loadImageFromCache(joda::enums::ImageId{.zProjection = enums::ZProjection::$, .imagePlane = {}})->image,
                context.getActImage().image, thumb, foundObjects};
      }
    }
  }
  thumbThread.join();
  DurationCount::stop(ii);
  return {{}, {}, {}, {}};
}

}    // namespace joda::processor
