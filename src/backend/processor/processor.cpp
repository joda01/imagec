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
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_clusters.hpp"
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
      auto imagesToProcess = db.prepareImages(plate.plateId, plate.groupBy, plate.filenameRegex, images);
      mProgress.setStateRunning();

      //
      // Iterate over each image
      //
      for(const auto &actImage : imagesToProcess) {
        auto analyzeImage = [this, &program, &globalContext, &plateContext, &pipelineOrder, &db, &poolSizeTiles,
                             &poolSizeChannels, &actImage]() {
          auto const [imagePath, omeInfo, imageId] = actImage;
          PipelineInitializer imageLoader(program.imageSetup);
          ImageContext imageContext{
              .imageLoader = imageLoader, .imagePath = imagePath, .imageMeta = omeInfo, .imageId = imageId};
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

              auto analyzeTile = [this, &program, &globalContext, &plateContext, &pipelineOrder, &db,
                                  imagePath = imagePath, nrtStack, nrzSTack, nrChannels, &imageContext, &imageLoader,
                                  tileX, tileY, &poolSizeChannels]() {
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
                    BS::multi_future<void> clustersFuture;
                    for(const auto &[order, pipelines] : pipelineOrder) {
                      auto executePipeline = [this, &program, &globalContext, &plateContext, &pipelineOrder, &db,
                                              imagePath, nrtStack, nrzSTack, &imageContext, &imageLoader, tileX, tileY,
                                              nrChannels, pipelines = pipelines, &iterationContext, tStack, zStack]() {
                        // These are pipelines in onw prio step -> Can be parallelized
                        for(const auto &pipeline : pipelines) {
                          if(mProgress.isStopping()) {
                            break;
                          }
                          //
                          // Load the image imagePlane
                          //
                          ProcessContext context{globalContext, plateContext, imageContext, iterationContext};
                          imageLoader.initPipeline(
                              pipeline->pipelineSetup, {tileX, tileY},
                              {.tStack = tStack, .zStack = zStack, .cStack = pipeline->pipelineSetup.cStackIndex},
                              context);
                          auto planeId = context.getActImage().getId().imagePlane;
                          try {
                            if(pipeline->pipelineSetup.cStackIndex >= 0 &&
                               pipeline->pipelineSetup.cStackIndex < nrChannels) {
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
                        }
                      };

                      if(poolSizeChannels > 1) {
                        clustersFuture.push_back(mGlobThreadPool.submit_task(executePipeline));
                      } else {
                        executePipeline();
                      }
                    }
                    if(poolSizeChannels > 1) {
                      clustersFuture.wait();
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
  } catch(const std::exception &ex) {
    mProgress.setStateError(mJobInformation, ex.what());
  }
}

std::string Processor::initializeGlobalContext(const joda::settings::AnalyzeSettings &program,
                                               const std::string &jobName, GlobalContext &globalContext)
{
  auto now = std::chrono::high_resolution_clock::now();
  mProgress.reset();
  globalContext.resultsOutputFolder = std::filesystem::path(program.projectSettings.workingDirectory) / "imagec" /
                                      (joda::helper::timepointToIsoString(now) + "_" + jobName);

  std::filesystem::create_directories(globalContext.resultsOutputFolder);

  settings::Settings::storeSettings((globalContext.resultsOutputFolder / "settings.icproj"), program);

  mJobInformation.resultsFilePath  = globalContext.resultsOutputFolder / "results.icdb";
  mJobInformation.ouputFolder      = globalContext.resultsOutputFolder;
  mJobInformation.jobName          = jobName;
  mJobInformation.timestampStarted = now;
  auto &db                         = globalContext.database;
  db.openDatabase(globalContext.resultsOutputFolder / "results.icdb");
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
auto Processor::generatePreview(const settings::ProjectImageSetup &imageSetup, const settings::Pipeline &pipeline,
                                const std::filesystem::path &imagePath, int32_t tStack, int32_t zStack, int32_t tileX,
                                int32_t tileY) -> std::tuple<cv::Mat, cv::Mat, cv::Mat>
{
  auto ome = joda::image::reader::ImageReader::getOmeInformation(imagePath);

  GlobalContext globalContext;
  PlateContext plateContext{.plateId = 0};
  joda::grp::FileGrouper grouper(enums::GroupBy::OFF, "");
  PipelineInitializer imageLoader(imageSetup);
  ImageContext imageContext{.imageLoader = imageLoader, .imagePath = imagePath, .imageMeta = ome, .imageId = 1};
  imageLoader.init(imageContext);

  IterationContext iterationContext;

  //
  // Load the image imagePlane
  //
  ProcessContext context{globalContext, plateContext, imageContext, iterationContext};
  imageLoader.initPipeline(pipeline.pipelineSetup, {tileX, tileY},
                           {.tStack = tStack, .zStack = zStack, .cStack = pipeline.pipelineSetup.cStackIndex}, context);
  auto planeId = context.getActImage().getId().imagePlane;

  //
  // Execute the pipeline
  //
  for(const auto &step : pipeline.pipelineSteps) {
    step(context, context.getActImage().image, context.getActObjects());
  }

  joda::settings::ImageSaverSettings saverSettings;

  for(int cluster = 0; cluster < 10; cluster++) {
    for(int classs = 0; classs < 10; classs++) {
      saverSettings.clustersIn.emplace_back(settings::ImageSaverSettings::SaveCluster{
          .inputCluster = {(enums::ClusterIdIn) cluster, (enums::ClassId) classs},
          .color        = "#FF0000",
          .style        = settings::ImageSaverSettings::Style::FILLED});
    }
  }

  saverSettings.canvas     = settings::ImageSaverSettings::Canvas::IMAGE_PLANE;
  saverSettings.planesIn   = enums::ImageId{.imageIdx = enums::ZProjection::$};
  saverSettings.outputSlot = settings::ImageSaverSettings::Output::IMAGE_$;
  auto step                = settings::PipelineStep{.$saveImage = saverSettings};
  auto saver               = joda::settings::PipelineFactory<joda::cmd::Command>::generate(step);
  saver->execute(context, context.getActImage().image, context.getActObjects());

  auto thumb = joda::image::reader::ImageReader::loadThumbnail(
      imagePath.string(),
      joda::image::reader::ImageReader::Plane{.z = zStack, .c = pipeline.pipelineSetup.cStackIndex, .t = tStack}, 0);

  return {context.loadImageFromCache(joda::enums::ImageId{.imageIdx = enums::ZProjection::$, .imagePlane = {}})->image,
          context.getActImage().image, thumb};
}

}    // namespace joda::processor
