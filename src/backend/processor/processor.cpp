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
#include <cstddef>
#include <exception>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include "backend/artifacts/roi/roi.hpp"
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
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/threading/threading.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/processor/dependency_graph.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/settings.hpp"
#include "backend/settings/settings_types.hpp"
#include <BS_thread_pool.hpp>

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

struct Task
{
public:
  /////////////////////////////////////////////////////
  // program.getProjectPath()
  Task(const GlobalContext *globCtx, const PipelineInitializer *imgCtx, const std::filesystem::path &projectPath, const PipelineOrder_t *pipeline,
       int32_t tileX, int32_t tileY, int32_t tStack, int32_t zStack) :
      globalContext(globCtx),
      imageContext(imgCtx), pipelineOrder(pipeline), mtileX(tileX), mtileY(tileY), mtStack(tStack), mzStack(zStack),
      objectCache(std::make_shared<joda::atom::ObjectList>()), iterationContext(objectCache, projectPath, imgCtx->getImagePath())
  {
  }

  void execute()
  {
    for(const auto &[order, pipelines] : *pipelineOrder) {
      for(const auto &pipelineToExecute : pipelines) {    // These are pipelines in one prio step -> Can be parallelized
        processPipeline(pipelineToExecute);
      }
    }

    globalContext->database->insertObjects(*imageContext, imageContext->getPixelSizeUnit(), iterationContext.getObjects());
  }

  void processPipeline(const joda::settings::Pipeline *pipelineToExecute)
  {
    DurationCount durationImagePipelineProcess("Process pipeline");
    ProcessContext context{*globalContext, *imageContext, iterationContext};
    imageContext->initPipeline(pipelineToExecute->pipelineSetup, {mtileX, mtileY},
                               {.tStack = mtStack, .zStack = mzStack, .cStack = pipelineToExecute->pipelineSetup.cStackIndex}, context,
                               pipelineToExecute->index);

    const auto planeId    = context.getActImage().getId().imagePlane;
    const auto nrChannels = imageContext->getNrOfChannels();
    if(pipelineToExecute->pipelineSetup.cStackIndex >= 0 && pipelineToExecute->pipelineSetup.cStackIndex < nrChannels) {
      DurationCount waitCounter("DB insert image plane");
      globalContext->database->insertImagePlane(imageContext->getImageId(), planeId,
                                                imageContext->getChannelInfos()
                                                    .at(static_cast<uint32_t>(pipelineToExecute->pipelineSetup.cStackIndex))
                                                    .planes.at(planeId.tStack)
                                                    .at(planeId.zStack));
    }

    // Execute the pipeline
    DurationCount durationCountPipelineSteps("Process pipeline steps");
    for(const auto &step : pipelineToExecute->pipelineSteps) {
      step(context, context.getActImage().image, context.getActObjects());
    }
    durationCountPipelineSteps.stop();
    iterationContext.removeTemporaryObjects(&context);
  }

private:
  /////////////////////////////////////////////////////
  const GlobalContext *globalContext;
  const PipelineInitializer *imageContext;
  const PipelineOrder_t *pipelineOrder;
  int32_t mtileX  = 0;
  int32_t mtileY  = 0;
  int32_t mtStack = 0;
  int32_t mzStack = 0;
  std::shared_ptr<joda::atom::ObjectList> objectCache;
  IterationContext iterationContext;
};

void Processor::execute(const joda::settings::AnalyzeSettings &program, const std::string &jobName,
                        const joda::thread::ThreadingSettings &threadingSettings, const std::unique_ptr<imagesList_t> &imagesToAnalyze)
{
  try {
    DurationCount::resetStats();
    // Prepare thread pool
    mGlobThreadPool.reset(threadingSettings.coresUsed);

    // Resolve dependencies
    auto pipelineOrder = joda::processor::DependencyGraph::calcGraph(program);

    // Prepare the context
    GlobalContext globalContext;
    auto jobId = initializeGlobalContext(program, jobName, globalContext);

    // Looking for images in all folders
    mProgress.setTotalNrOfImages(static_cast<uint32_t>(imagesToAnalyze->getNrOfFiles()));

    //
    // Iterate over each plate and analyze the images
    //
    const auto &plate  = program.projectSettings.plate;
    const auto &images = imagesToAnalyze->getFilesListAt();

    mProgress.setRunningPreparingPipeline();
    auto imagesToProcess = globalContext.database->prepareImages(plate.plateId, program.imageSetup.series, plate.groupBy, plate.filenameRegex, images,
                                                                 imagesToAnalyze->getDirectoryAt(), program, mGlobThreadPool);

    //
    // Iterate over each image
    //
    int32_t nrOfTiles = 0;
    std::vector<std::unique_ptr<Task>> tasks;
    for(const auto &actImage : imagesToProcess) {
      const auto [tilesX, tilesY] = actImage->getNrOfTilesToProcess();
      const auto nrtStack         = actImage->getNrOfTStacksToProcess();
      const auto nrzSTack         = actImage->getNrOfZStacksToProcess();

      for(int32_t tileX = 0; tileX < tilesX; tileX++) {
        for(int32_t tileY = 0; tileY < tilesY; tileY++) {
          // Start of the image specific function
          int32_t tStackStart = 0;
          auto tStackEnd      = static_cast<int32_t>(nrtStack);
          if(program.imageSetup.tStackSettings.startFrame > static_cast<int32_t>(nrtStack)) {
            tStackStart = static_cast<int32_t>(nrtStack);
          } else {
            tStackStart = program.imageSetup.tStackSettings.startFrame;
          }
          if(program.imageSetup.tStackSettings.endFrame >= 0 && program.imageSetup.tStackSettings.endFrame <= static_cast<int32_t>(nrtStack)) {
            tStackEnd = program.imageSetup.tStackSettings.endFrame;
          }
          for(int32_t tStack = tStackStart; tStack < tStackEnd; tStack++) {
            for(int32_t zStack = 0; zStack < static_cast<int32_t>(nrzSTack); zStack++) {
              tasks.emplace_back(
                  std::make_unique<Task>(&globalContext, actImage.get(), program.getProjectPath(), &pipelineOrder, tilesX, tilesY, tStack, zStack));
              nrOfTiles++;
            }
          }
        }
      }

      // Image finished
      // db->setImageProcessed(imageContext.imageId);
      // mProgress.incProcessedImages();
    }
    mProgress.setTotalNrOfImages(imagesToProcess.size());
    mProgress.setTotalNrOfTiles(nrOfTiles);
    mProgress.setStateRunning();

    uint32_t addedTasks = 0;
    for(const auto &taskToProcess : tasks) {
      if(addedTasks >= threadingSettings.coresUsed) {
        addedTasks = 0;
        mGlobThreadPool.wait();
      }
      mGlobThreadPool.submit_task([this, &taskToProcess]() {
        taskToProcess->execute();
        mProgress.incProcessedTiles();
      });
      addedTasks++;
    }

    mGlobThreadPool.wait();

    // Done
    globalContext.database->finishJob(jobId);
    globalContext.database->closeDatabase();
    mProgress.setStateFinished();
    DurationCount::printStats(static_cast<int32_t>(imagesToAnalyze->getNrOfFiles()), mJobInformation.ouputFolder);
  } catch(const std::exception &ex) {
    mProgress.setStateError(mJobInformation, ex.what());
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::string Processor::initializeGlobalContext(const joda::settings::AnalyzeSettings &program, const std::string &jobName,
                                               GlobalContext &globalContext)
{
  auto now = std::chrono::system_clock::now();
  mProgress.reset();
  globalContext.classes.clear();
  for(const auto &elem : program.projectSettings.classification.classes) {
    globalContext.classes.emplace(elem.classId, elem);
  }

  globalContext.resultsOutputFolder = std::filesystem::path(program.projectSettings.plate.imageFolder) / joda::fs::WORKING_DIRECTORY_PROJECT_PATH /
                                      joda::fs::RESULTS_PATH / (joda::helper::timepointToIsoString(now) + "_" + jobName);
  globalContext.workingDirectory = program.getProjectPath();

  std::filesystem::create_directories(globalContext.resultsOutputFolder);

  //
  // Make a copy of the pipeline settings to the output folder
  //
  settings::Settings::storeSettings((globalContext.resultsOutputFolder / (joda::fs::FILE_NAME_PROJECT_DEFAULT + joda::fs::EXT_PROJECT)), program);

  // Copy models
  {
    std::filesystem::path source      = program.getProjectPath() / joda::fs::WORKING_DIRECTORY_MODELS_PATH;
    std::filesystem::path destination = globalContext.resultsOutputFolder / joda::fs::WORKING_DIRECTORY_MODELS_PATH;

    try {
      std::filesystem::create_directories(destination);
      std::filesystem::copy(source, destination, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
    } catch(const std::filesystem::filesystem_error &e) {
      joda::log::logWarning("Could not copy models to output path what: " + std::string(e.what()));
    }
  }

  // Copy ROIs
  {
    std::filesystem::path source      = program.getProjectPath() / joda::fs::WORKING_DIRECTORY_IMAGE_DATA_PATH;
    std::filesystem::path destination = globalContext.resultsOutputFolder / joda::fs::WORKING_DIRECTORY_IMAGE_DATA_PATH;

    try {
      std::filesystem::create_directories(destination);
      std::filesystem::copy(source, destination, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
    } catch(const std::filesystem::filesystem_error &e) {
      joda::log::logWarning("Could not copy data to output path what: " + std::string(e.what()));
    }
  }

  //
  // Job settings
  //
  mJobInformation.resultsFilePath  = globalContext.resultsOutputFolder / (joda::fs::FILE_NAME_RESULTS_DATABASE + joda::fs::EXT_DATABASE);
  mJobInformation.ouputFolder      = globalContext.resultsOutputFolder;
  mJobInformation.jobName          = jobName;
  mJobInformation.timestampStarted = now;
  globalContext.database           = std::make_unique<db::Database>();
  globalContext.database->openDatabase(globalContext.resultsOutputFolder / (joda::fs::FILE_NAME_RESULTS_DATABASE + joda::fs::EXT_DATABASE));
  return globalContext.database->startJob(program, jobName);
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
                                int32_t tileX, int32_t tileY, const ome::OmeInfo &ome, Preview &previewOut) -> void
{
  DurationCount durationCount("Generate preview.");

  // Prepare thread pool
  mGlobThreadPool.reset(threadingSettings.coresUsed);
  auto poolSizeChannels = threadingSettings.coresUsed;

  //
  //  Resolve dependencies
  //  We only want to execute those pipelines which are needed for the preview
  //
  auto pipelineOrder = joda::processor::DependencyGraph::calcGraph(program, &pipelineStart);
  if(pipelineOrder.empty()) {
    throw std::invalid_argument("Cycle detected in pipelines!");
  }

  //
  // Get image
  //
  GlobalContext globalContext;
  globalContext.resultsOutputFolder = program.getProjectPath() / joda::fs::RESULTS_PATH / "preview";
  globalContext.workingDirectory    = program.getProjectPath();

  globalContext.database = std::make_unique<db::PreviewDatabase>();
  auto *db               = dynamic_cast<db::PreviewDatabase *>(globalContext.database.get());

  globalContext.classes.clear();
  for(const auto &elem : program.projectSettings.classification.classes) {
    globalContext.classes.emplace(elem.classId, elem);
  }

  joda::grp::FileGrouper grouper(enums::GroupBy::OFF, "");
  PipelineInitializer imageLoader(program.imageSetup, program.pipelineSetup, imagePath, program.getProjectPath());

  auto objectMapBuffer = std::make_shared<joda::atom::ObjectList>();
  IterationContext iterationContext(objectMapBuffer, program.getProjectPath(), imagePath);

  size_t totalRuns = 0;
  for(const auto &[order, pipelines] : pipelineOrder) {
    totalRuns += pipelines.size();
  }

  //  std::tuple<cv::Mat, cv::Mat, cv::Mat, enums::ChannelValidity, joda::atom::ObjectList> tmpResult;
  bool finished = false;

  size_t executedSteps = 0;
  for(const auto &[order, pipelines] : pipelineOrder) {
    BS::multi_future<void> pipelinesFutures;
    for(const auto &pipelineToExecute : pipelines) {
      executedSteps++;
      if(executedSteps > totalRuns) {
        continue;
      }

      auto executePipeline = [&db, &finished, &ome, &previewOut, &imageSetup, &totalRuns, pipeline = pipelineToExecute, &globalContext, imagePath,
                              &imageLoader, tileX, tileY, pipelines = pipelines, &iterationContext, tStack, zStack, executedSteps]() -> void {
        //
        // The last step is the wanted pipeline
        //
        bool wantedPipeline = executedSteps >= totalRuns;

        //
        // Load the image imagePlane
        //
        ProcessContext context{globalContext, imageLoader, iterationContext};
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
          if(step.breakPoint && wantedPipeline) {
            editedImageAtBreakpoint = context.getActImage().image.clone();
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
        if(wantedPipeline) {
          // No breakpoint was set, use the last image
          if(editedImageAtBreakpoint.empty()) {
            editedImageAtBreakpoint = context.getActImage().image.clone();
          }

          //
          // Finished
          //
          previewOut.editedImage.setImage(editedImageAtBreakpoint,
                                          ome.getPseudoColorForChannel(imageSetup.series, pipeline->pipelineSetup.cStackIndex));
          previewOut.results.noiseDetected = db->getImageValidity().test(enums::ChannelValidityEnum::POSSIBLE_NOISE);
          previewOut.results.isOverExposed = db->getImageValidity().test(enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
          previewOut.tStacks               = ome.getNrOfTStack(imageSetup.series);

          finished = true;
        }
      };

      if(poolSizeChannels > 1) {
        pipelinesFutures.push_back(mGlobThreadPool.submit_task(executePipeline));
      } else {
        executePipeline();
      }
    }
    {
      DurationCount waitCounter("Waiting for pipeline finished");
      if(poolSizeChannels > 1) {
        pipelinesFutures.wait();
      }
    }
  }

  previewOut.results.objectMap->triggerStartChangeCallback();
  previewOut.results.objectMap->mergeFrom(std::move(*objectMapBuffer), {});
  previewOut.results.objectMap->triggerChangeCallback();
  objectMapBuffer.reset();
}

}    // namespace joda::processor
