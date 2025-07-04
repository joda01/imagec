///
/// \file      processor.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "processor.hpp"
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/database/database.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/enums/enums_grouping.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
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
#include "backend/settings/settings_types.hpp"

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
      auto imagesToProcess = db->prepareImages(plate.plateId, program.imageSetup.series, plate.groupBy, plate.filenameRegex, images,
                                               allImages.getDirectoryAt(plate.plateId), mGlobThreadPool);
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
          auto nrChannels       = omeInfo.getNrOfChannels(imageContext.series);

          mProgress.setTotalNrOfTiles(mProgress.totalImages() * tilesX * tilesY * nrtStack);
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
                int32_t tStackStart = 0;
                auto tStackEnd      = static_cast<int32_t>(nrtStack);
                if(program.imageSetup.tStackSettings.startFrame > nrtStack) {
                  tStackStart = static_cast<int32_t>(nrtStack);
                } else {
                  tStackStart = program.imageSetup.tStackSettings.startFrame;
                }
                if(program.imageSetup.tStackSettings.endFrame >= 0 && program.imageSetup.tStackSettings.endFrame <= nrtStack) {
                  tStackEnd = program.imageSetup.tStackSettings.endFrame;
                }
                for(int tStack = tStackStart; tStack < tStackEnd; tStack++) {
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
                                                     {.tStack = tStack, .zStack = zStack, .cStack = pipeline->pipelineSetup.cStackIndex}, context,
                                                     pipeline->index);
                            auto planeId = context.getActImage().getId().imagePlane;
                            try {
                              if(pipeline->pipelineSetup.cStackIndex >= 0 && pipeline->pipelineSetup.cStackIndex < nrChannels) {
                                db->insertImagePlane(imageContext.imageId, planeId,
                                                     imageContext.imageMeta.getChannelInfos(imageContext.series)
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

                            // Remove temporary objects from pipeline
                            joda::log::logTrace("Pipeline >" + pipeline->meta.name + "< finished!");
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_01));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_02));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_03));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_04));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_05));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_06));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_07));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_08));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_09));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_10));
                            iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_11));
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

                    // Insert objects to database
                    auto id = DurationCount::start("Insert");
                    try {
                      db->insertObjects(imageContext, iterationContext.getObjects());
                    } catch(const std::exception &ex) {
                      std::cout << "Insert Obj: " << ex.what() << std::endl;
                    }
                    DurationCount::stop(id);
                  }

                  // Time frame finished
                  mProgress.incProcessedTiles();
                }
                // Tile finished
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
          db->setImageProcessed(imageContext.imageId);
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
    db->finishJob(jobId);
    globalContext.database->closeDatabase();
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
  globalContext.classes.clear();
  for(const auto &elem : program.projectSettings.classification.classes) {
    globalContext.classes.emplace(elem.classId, elem);
  }

  globalContext.resultsOutputFolder =
      std::filesystem::path(program.projectSettings.workingDirectory) / "imagec" / (joda::helper::timepointToIsoString(now) + "_" + jobName);

  std::filesystem::create_directories(globalContext.resultsOutputFolder);

  settings::Settings::storeSettings((globalContext.resultsOutputFolder / ("settings" + joda::fs::EXT_PROJECT)), program);

  mJobInformation.resultsFilePath  = globalContext.resultsOutputFolder / ("results" + joda::fs::EXT_DATABASE);
  mJobInformation.ouputFolder      = globalContext.resultsOutputFolder;
  mJobInformation.jobName          = jobName;
  mJobInformation.timestampStarted = now;
  globalContext.database           = std::make_unique<db::Database>();
  globalContext.database->openDatabase(globalContext.resultsOutputFolder / ("results" + joda::fs::EXT_DATABASE));
  return globalContext.database->startJob(program, jobName);
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
                                const settings::AnalyzeSettings &program, const joda::thread::ThreadingSettings &threadingSettings,
                                const settings::Pipeline &pipelineStart, const std::filesystem::path &imagePath, int32_t tStack, int32_t zStack,
                                int32_t tileX, int32_t tileY, bool generateThumb, const ome::OmeInfo &ome,
                                const settings::ObjectInputClasses &classesToShow)
    -> std::tuple<cv::Mat, cv::Mat, cv::Mat, cv::Mat, std::map<joda::enums::ClassId, PreviewReturn>, enums::ChannelValidity>
{
  auto ii = DurationCount::start("Generate preview with >" + std::to_string(threadingSettings.coresUsed) + "< threads.");

  // Prepare thread pool
  mGlobThreadPool.reset(threadingSettings.coresUsed);
  auto poolSizeChannels = threadingSettings.coresUsed;

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
          imagePath.string(), joda::image::reader::ImageReader::Plane{.z = zStack, .c = pipelineStart.pipelineSetup.cStackIndex, .t = tStack},
          imageSetup.series, ome);
      DurationCount::stop(i);
    }
  });

  //
  // Get image
  //
  GlobalContext globalContext;
  globalContext.database = std::make_unique<db::PreviewDatabase>();
  auto *db               = dynamic_cast<db::PreviewDatabase *>(globalContext.database.get());

  globalContext.classes.clear();
  for(const auto &elem : program.projectSettings.classification.classes) {
    globalContext.classes.emplace(elem.classId, elem);
  }

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

  std::tuple<cv::Mat, cv::Mat, cv::Mat, cv::Mat, std::map<joda::enums::ClassId, PreviewReturn>, enums::ChannelValidity> tmpResult;
  bool finished = false;

  int executedSteps = 0;
  for(const auto &[order, pipelines] : pipelineOrder) {
    BS::multi_future<void> pipelinesFutures;
    for(const auto &pipelineToExecute : pipelines) {
      executedSteps++;
      if(executedSteps > totalRuns) {
        continue;
      }

      auto executePipeline = [&db, &thumbThread, &thumb, &finished, &tmpResult, &previewSettings, &pipelineStart, &classesToShow, &totalRuns,
                              pipeline = pipelineToExecute, this, &program, &globalContext, &plateContext, &pipelineOrder, imagePath, &imageContext,
                              &imageLoader, tileX, tileY, pipelines = pipelines, &iterationContext, tStack, zStack, executedSteps]() -> void {
        //
        // The last step is the wanted pipeline
        //
        bool previewPipeline = executedSteps >= totalRuns;

        //
        // Load the image imagePlane
        //
        ProcessContext context{globalContext, plateContext, imageContext, iterationContext};
        imageLoader.initPipeline(pipeline->pipelineSetup, {tileX, tileY},
                                 {.tStack = tStack, .zStack = zStack, .cStack = pipeline->pipelineSetup.cStackIndex}, context, pipeline->index);
        auto planeId = context.getActImage().getId().imagePlane;

        //
        // Execute the pipeline
        //
        cv::Mat editedImageAtBreakpoint;
        for(const auto &step : pipeline->pipelineSteps) {
          if(step.$saveImage.has_value()) {
            // For preview do not execute image saver
            continue;
          }
          // Breakpoints are only enabled in the preview pipeline
          if(step.breakPoint && previewPipeline) {
            editedImageAtBreakpoint = context.getActImage().image.clone();
            // break;
          }
          step(context, context.getActImage().image, context.getActObjects());
        }
        // Remove temporary objects from pipeline
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_01));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_02));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_03));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_04));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_05));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_06));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_07));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_08));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_09));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_10));
        iterationContext.getObjects().erase(context.getTemporaryClassId(enums::ClassIdIn::TEMP_11));

        //
        // The last step is the wanted pipeline
        //
        if(previewPipeline) {
          //
          // Count elements
          //
          std::map<joda::enums::ClassId, PreviewReturn> foundObjects;
          {
            for(auto const &[classs, objects] : context.getActObjects()) {
              for(const auto &roi : *objects) {
                joda::enums::ClassId key = static_cast<enums::ClassId>(roi.getClassId());
                if(!foundObjects.contains(key)) {
                  foundObjects[key].count       = 0;
                  foundObjects[key].color       = "#BFBFBF";
                  foundObjects[key].wantedColor = globalContext.classes[key].color;
                }
                foundObjects[key].count++;
              }
            }
          }

          //
          // Generate preview image
          //
          joda::settings::ImageSaverSettings saverSettings;
          saverSettings.classesIn.clear();
          for(enums::ClassIdIn classs : classesToShow) {
            if(classs >= enums::ClassIdIn::TEMP_01 && classs <= enums::ClassIdIn::TEMP_LAST) {
              /// \todo allow to show temp in preview
              continue;
            }
            if(classs == enums::ClassIdIn::$) {
              classs = static_cast<enums::ClassIdIn>(pipelineStart.pipelineSetup.defaultClassId);
            }

            auto key = static_cast<enums::ClassId>(classs);
            if(foundObjects.contains(key)) {
              // Objects which are selected should be painted in color in the legend, not selected are black
              foundObjects[key].color = foundObjects.at(key).wantedColor;

              saverSettings.classesIn.emplace_back(settings::ImageSaverSettings::SaveClasss{
                  .inputClass = classs, .style = previewSettings.style, .paintBoundingBox = false, .paintObjectId = false});
            }
          }
          // No breakpoint was set, use the last image
          if(editedImageAtBreakpoint.empty()) {
            editedImageAtBreakpoint = context.getActImage().image.clone();
          }
          saverSettings.canvas     = settings::ImageSaverSettings::Canvas::BLACK;
          saverSettings.planesIn   = enums::ImageId{.zProjection = enums::ZProjection::$};
          saverSettings.outputSlot = settings::ImageSaverSettings::Output::IMAGE_$;
          auto step                = settings::PipelineStep{.$saveImage = saverSettings};
          auto saver               = joda::settings::PipelineFactory<joda::cmd::Command>::generate(step);
          saver->execute(context, context.getActImage().image, context.getActObjects());
          ///\warning #warning "Exception on thread destructor"
          thumbThread.join();

          tmpResult = {
              context.loadImageFromCache(enums::MemoryScope::ITERATION, joda::enums::ImageId{.zProjection = enums::ZProjection::$, .imagePlane = {}})
                  ->image,
              context.getActImage().image,
              editedImageAtBreakpoint,
              thumb,
              foundObjects,
              db->getImageValidity()};
          finished = true;
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
  }

  if(!finished) {
    thumbThread.join();
    DurationCount::stop(ii);
    return {{}, {}, {}, {}, {}, {}};
  } else {
    DurationCount::stop(ii);
    return tmpResult;
  }
}

}    // namespace joda::processor
