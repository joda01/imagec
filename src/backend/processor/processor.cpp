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
#include <type_traits>
#include <vector>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/database/database.hpp"
#include "backend/database/database_interface.hpp"
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
#include "backend/helper/system/system_resources.hpp"
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
  mCancelAll.store(true);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
template <bool PREVIEW_TASK = false>
struct Task
{
public:
  /////////////////////////////////////////////////////
  // program.getProjectPath()
  Task(ProcessProgress *progress, const GlobalContext *globCtx, const PipelineInitializer *imgCtx, const std::filesystem::path &projectPath,
       const PipelineOrder_t *pipeline, int32_t tileX, int32_t tileY, int32_t tStack, int32_t zStack) :
      mProgress(progress),
      globalContext(globCtx), imageContext(imgCtx), pipelineOrder(pipeline), mtileX(tileX), mtileY(tileY), mtStack(tStack), mzStack(zStack),
      objectCache(std::make_shared<joda::atom::ObjectList>()), iterationContext(objectCache, projectPath, imgCtx->getImagePath())
  {
  }

  void execute(BS::thread_pool<> *threadPool = nullptr, const settings::Pipeline *previewPipeline = nullptr)
  {
    mPreviewPipeline = previewPipeline;
    for(const auto &[order, pipelines] : *pipelineOrder) {
      for(const auto &pipelineToExecute : pipelines) {
        if constexpr(PREVIEW_TASK) {
          // In preview task we parallelize the pipeline execution
          (void) threadPool->submit_task([this, pipelineToExecute]() { processPipeline(pipelineToExecute); });
        } else {
          processPipeline(pipelineToExecute);
        }
      }
      if constexpr(PREVIEW_TASK) {
        threadPool->wait();
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
      if constexpr(PREVIEW_TASK) {
        if(mPreviewPipeline == pipelineToExecute && step.breakPoint) {
          // Breakpoints only allowed in the pipeline for which the preview should be generated for
          editedImageAtBreakpoint = context.getActImage().image.clone();
        }
      }
      step(context, context.getActImage().image, context.getActObjects());
      mProgress->incProcessedPipelineSteps();
    }

    // Pipeline finished
    durationCountPipelineSteps.stop();
    iterationContext.removeTemporaryObjects(&context);

    if constexpr(PREVIEW_TASK) {
      if(mPreviewPipeline == pipelineToExecute && editedImageAtBreakpoint.empty()) {
        // Breakpoints only allowed in the pipeline for which the preview should be generated for
        editedImageAtBreakpoint = context.getActImage().image.clone();
      }
    }
  }

  [[nodiscard]] auto getObjects() const -> std::shared_ptr<joda::atom::ObjectList>
  {
    return objectCache;
  }

  [[nodiscard]] auto getEditedImageAtBreakpoint() const -> const cv::Mat &
  {
    return editedImageAtBreakpoint;
  }

private:
  /////////////////////////////////////////////////////
  ProcessProgress *mProgress;
  const GlobalContext *globalContext;
  const PipelineInitializer *imageContext;
  const PipelineOrder_t *pipelineOrder;
  int32_t mtileX  = 0;
  int32_t mtileY  = 0;
  int32_t mtStack = 0;
  int32_t mzStack = 0;
  std::shared_ptr<joda::atom::ObjectList> objectCache;
  IterationContext iterationContext;
  cv::Mat editedImageAtBreakpoint;
  const settings::Pipeline *mPreviewPipeline = nullptr;
};

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Processor::execute(std::unique_ptr<BS::thread_pool<>> &threadPool, const joda::settings::AnalyzeSettings &program, const std::string &jobName,
                        const std::unique_ptr<imagesList_t> &imagesToAnalyze)
{
  try {
    mCancelAll.store(false);
    mProgress.setRunningPreparingPipeline();

    DurationCount::resetStats();
    // Resolve dependencies
    auto pipelineOrder = joda::processor::DependencyGraph::calcGraph(program);
    mGlobalContext     = initializeGlobalContext<db::Database>(program, jobName);
    prepareOutputFolder(program, mGlobalContext);

    const auto &plate          = program.projectSettings.plate;
    const auto &images         = imagesToAnalyze->getFilesListAt();
    const auto imagesToProcess = mGlobalContext->database->prepareImages(plate.plateId, program.imageSetup.series, plate.groupBy, plate.filenameRegex,
                                                                         images, imagesToAnalyze->getDirectoryAt(), program, threadPool);

    //
    // Prepare the tasks to execute
    //
    int32_t nrOfTiles = 0;
    // std::vector<std::unique_ptr<Task<false>>> tasks;
    for(const auto &actImage : imagesToProcess) {
      const auto [tilesX, tilesY] = actImage->getNrOfTilesToProcess();
      const auto nrtStack         = actImage->getNrOfTStacksToProcess();
      const auto nrzSTack         = actImage->getNrOfZStacksToProcess();
      const int32_t lastTileX     = tilesX - 1;
      const int32_t lastTileY     = tilesY - 1;

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
          const int32_t lastTStack = tStackEnd - 1;

          for(int32_t tStack = tStackStart; tStack < tStackEnd; tStack++) {
            const int32_t lastZStack = static_cast<int32_t>(nrzSTack) - 1;

            for(int32_t zStack = 0; zStack < static_cast<int32_t>(nrzSTack); zStack++) {
              const bool lastTileOfImage = (tStack == lastTStack) && (zStack == lastZStack) && (tileX == lastTileX) && (tileY == lastTileY);

              (void) threadPool->submit_task([this, &program, &pipelineOrder, actImage, tileX, tileY, tStack, zStack, lastTileOfImage]() {
                if(mCancelAll.load(std::memory_order_relaxed)) {
                  return;
                }
                // Execute task
                std::unique_ptr<Task<false>> taskToExecute = std::make_unique<Task<false>>(
                    &mProgress, mGlobalContext.get(), actImage.get(), program.getProjectPath(), &pipelineOrder, tileX, tileY, tStack, zStack);
                taskToExecute->execute();
                mProgress.incProcessedTiles();

                // Image finished
                if(lastTileOfImage) {
                  mGlobalContext->database->setImageProcessed(actImage->getImageId());
                  mProgress.incProcessedImages();
                }
              });
              nrOfTiles++;
            }
          }
        }
      }
    }
    mProgress.setTotalNrOfImages(static_cast<uint32_t>(imagesToProcess.size()));
    mProgress.setTotalNrOfTiles(static_cast<uint32_t>(nrOfTiles));
    mProgress.setStateRunning();

    threadPool->wait();

    //
    // Done
    //
    mGlobalContext->database->finishJob(mGlobalContext->jobId);
    mGlobalContext->database->closeDatabase();
    mProgress.setStateFinished();
    DurationCount::printStats(static_cast<int32_t>(imagesToAnalyze->getNrOfFiles()), mGlobalContext->resultsOutputFolder);
  } catch(const std::exception &ex) {
    mProgress.setStateError(ex.what());
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Processor::generatePreview(std::unique_ptr<BS::thread_pool<>> &threadPool, const PreviewSettings & /*previewSettings*/,
                                const settings::ProjectImageSetup &imageSetup, const settings::AnalyzeSettings &program,
                                const settings::Pipeline &pipelineStart, const std::filesystem::path &imagePath, int32_t tStack, int32_t zStack,
                                int32_t tileX, int32_t tileY, const ome::OmeInfo &ome, Preview &previewOut) -> void
{
  DurationCount durationCount("Generate preview.");

  //
  //  Resolve dependencies
  //  We only want to execute those pipelines which are needed for the preview
  //
  const auto pipelineOrder = joda::processor::DependencyGraph::calcGraph(program, &pipelineStart);
  if(pipelineOrder.empty()) {
    throw std::invalid_argument("Cycle detected in pipelines!");
  }

  auto globalContext = initializeGlobalContext<db::PreviewDatabase>(program, "preview");
  PipelineInitializer imageLoader(program.imageSetup, program.pipelineSetup, imagePath, program.getProjectPath());

  //
  // Create the preview task
  //
  std::unique_ptr<Task<true>> task = std::make_unique<Task<true>>(&mProgress, globalContext.get(), &imageLoader, program.getProjectPath(),
                                                                  &pipelineOrder, tileX, tileY, tStack, zStack);

  task->execute(threadPool.get(), &pipelineStart);

  //
  // Finished assign results
  //
  previewOut.editedImage.setImage(task->getEditedImageAtBreakpoint(),
                                  ome.getPseudoColorForChannel(imageSetup.series, pipelineStart.pipelineSetup.cStackIndex));
  previewOut.results.noiseDetected = globalContext->database->getImageValidity().test(enums::ChannelValidityEnum::POSSIBLE_NOISE);
  previewOut.results.isOverExposed = globalContext->database->getImageValidity().test(enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
  previewOut.tStacks               = ome.getNrOfTStack(imageSetup.series);

  previewOut.results.objectMap->triggerStartChangeCallback();
  previewOut.results.objectMap->mergeFrom(std::move(*task->getObjects()), {});
  previewOut.results.objectMap->triggerChangeCallback();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
template <class DATABASE_TYPE>
std::unique_ptr<GlobalContext> Processor::initializeGlobalContext(const joda::settings::AnalyzeSettings &program, const std::string &jobName)
{
  std::unique_ptr<GlobalContext> globalContext = std::make_unique<GlobalContext>();

  const auto now = std::chrono::system_clock::now();
  mProgress.reset();
  globalContext->classes.clear();
  for(const auto &elem : program.projectSettings.classification.classes) {
    globalContext->classes.emplace(elem.classId, elem);
  }
  globalContext->workingDirectory = program.getProjectPath();

  if constexpr(std::is_base_of_v<db::Database, DATABASE_TYPE>) {
    globalContext->resultsOutputFolder = std::filesystem::path(program.projectSettings.plate.imageFolder) / joda::fs::WORKING_DIRECTORY_PROJECT_PATH /
                                         joda::fs::RESULTS_PATH / (joda::helper::timepointToIsoString(now) + "_" + jobName);
  } else if constexpr(std::is_base_of_v<db::PreviewDatabase, DATABASE_TYPE>) {
    globalContext->resultsOutputFolder = program.getProjectPath() / joda::fs::RESULTS_PATH / jobName;
  }

  std::filesystem::create_directories(globalContext->resultsOutputFolder);

  globalContext->resultsDatabaseFilePath = globalContext->resultsOutputFolder / (joda::fs::FILE_NAME_RESULTS_DATABASE + joda::fs::EXT_DATABASE);

  globalContext->database = std::make_unique<DATABASE_TYPE>();
  globalContext->database->openDatabase(globalContext->resultsOutputFolder / (joda::fs::FILE_NAME_RESULTS_DATABASE + joda::fs::EXT_DATABASE));
  globalContext->jobId            = globalContext->database->startJob(program, jobName);
  globalContext->jobName          = jobName;
  globalContext->timestampStarted = now;

  return globalContext;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Processor::prepareOutputFolder(const joda::settings::AnalyzeSettings &program, const std::unique_ptr<GlobalContext> &globalContext) const
{
  //
  // Make a copy of the pipeline settings to the output folder
  //
  settings::Settings::storeSettings((globalContext->resultsOutputFolder / (joda::fs::FILE_NAME_PROJECT_DEFAULT + joda::fs::EXT_PROJECT)), program);

  // Copy models
  {
    std::filesystem::path source      = program.getProjectPath() / joda::fs::WORKING_DIRECTORY_MODELS_PATH;
    std::filesystem::path destination = globalContext->resultsOutputFolder / joda::fs::WORKING_DIRECTORY_MODELS_PATH;

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
    std::filesystem::path destination = globalContext->resultsOutputFolder / joda::fs::WORKING_DIRECTORY_IMAGE_DATA_PATH;

    try {
      std::filesystem::create_directories(destination);
      std::filesystem::copy(source, destination, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
    } catch(const std::filesystem::filesystem_error &e) {
      joda::log::logWarning("Could not copy data to output path what: " + std::string(e.what()));
    }
  }
}

}    // namespace joda::processor
