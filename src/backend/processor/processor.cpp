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
#include <filesystem>
#include <memory>
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/processor/context/plate_context.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/setting.hpp"

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
}

void Processor::execute(const joda::settings::AnalyzeSettings &program, imagesList_t &allImages)
{
  GlobalContext globalContext;
  auto jobId = initializeGlobalContext(program, globalContext);

  // Looking for images in all folders
  listImages(program, allImages);

  //
  // Iterate over each plate and analyze the images
  //
  auto &db = globalContext.database;
  for(const auto &plate : program.projectSettings.plates) {
    PlateContext plateContext{.plateId = plate.plateId};
    joda::grp::FileGrouper grouper(plate.groupBy, plate.filenameRegex);
    const auto &images = allImages.getFilesListAt(plate.plateId);

    //
    // Iterate over each image
    //
    for(const auto &imagePath : images) {
      PipelineInitializer imageLoader(program.imageSetup);
      ImageContext imageContext{.imageLoader = imageLoader};
      initializePipelineContext(program, globalContext, plateContext, grouper, imagePath, imageLoader, imageContext);

      //
      // Start the iteration over planes
      //
      auto [tilesX, tilesY] = imageLoader.getNrOfTilesToProcess();
      auto nrtStack         = imageLoader.getNrOfTStacksToProcess();
      auto nrzSTack         = imageLoader.getNrOfZStacksToProcess();

      for(int tileX = 0; tileX < tilesX; tileX++) {
        if(mProgress.isStopping()) {
          break;
        }
        for(int tileY = 0; tileY < tilesY; tileY++) {
          if(mProgress.isStopping()) {
            break;
          }
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
              for(const auto &pipeline : program.pipelines) {
                if(mProgress.isStopping()) {
                  break;
                }
                //
                // Load the image imagePlane
                //
                ProcessContext context{globalContext, plateContext, imageContext, iterationContext};
                imageLoader.initPipeline(
                    pipeline.pipelineSetup, {tilesX, tileY},
                    {.tStack = tStack, .zStack = zStack, .cStack = pipeline.pipelineSetup.cStackIndex}, context);
                auto planeId = context.getActImage().getId().imagePlane;
                db.insertImagePlane(imageContext.imageId, planeId,
                                    imageContext.imageMeta.getChannelInfos()
                                        .at(pipeline.pipelineSetup.cStackIndex)
                                        .planes.at(planeId.tStack)
                                        .at(planeId.zStack));

                //
                // Execute the pipeline
                //
                for(const auto &step : pipeline.pipelineSteps) {
                  if(mProgress.isStopping()) {
                    break;
                  }
                  // Execute a pipeline step
                  step(context, context.getActImage().image, context.getActObjects());
                }
              }

              // Iteration for all tiles finished
              auto id = DurationCount::start("Insert");
              db.insertObjects(imageContext, iterationContext.getObjects());
              DurationCount::stop(id);
            }
          }
          // Tile finished
          mProgress.incProcessedTiles();
        }
        // Image finished
        mProgress.incProcessedImages();
      }
    }
  }

  // Done
  db.finishJob(jobId);
  mProgress.setStateFinished();
}

std::string Processor::initializeGlobalContext(const joda::settings::AnalyzeSettings &program,
                                               GlobalContext &globalContext)
{
  mProgress.reset();
  globalContext.resultsOutputFolder = std::filesystem::path(program.projectSettings.workingDirectory);

  auto &db = globalContext.database;
  db.openDatabase(std::filesystem::path(program.projectSettings.workingDirectory) / "results.imcdb");
  return db.startJob(program);
}

void Processor::listImages(const joda::settings::AnalyzeSettings &program, imagesList_t &allImages)
{
  mProgress.setStateLookingForImages();
  for(const auto &plate : program.projectSettings.plates) {
    allImages.setWorkingDirectory(plate.plateId, plate.imageFolder);
  }
  allImages.waitForFinished();
  mProgress.setTotalNrOfImages(allImages.getNrOfFiles());
  mProgress.setStateRunning();
}

void Processor::initializePipelineContext(const joda::settings::AnalyzeSettings &program,
                                          const GlobalContext &globalContext, const PlateContext &plateContext,
                                          joda::grp::FileGrouper &grouper, const joda::filesystem::path &imagePath,
                                          PipelineInitializer &imageLoader, ImageContext &imageContext)
{
  auto &db = const_cast<joda::db::Database &>(globalContext.database);
  imageLoader.init(imagePath, imageContext, globalContext);

  //
  // Assign image to group here!!
  //
  auto groupInfo = grouper.getGroupForFilename(imagePath);
  db.insertGroup(plateContext.plateId, groupInfo);
  db.insertImage(imageContext, groupInfo);
  db.insertImageChannels(imageContext.imageId, imageContext.imageMeta);
  db.insetImageToGroup(plateContext.plateId, imageContext.imageId, groupInfo.imageIdx, groupInfo);
}

}    // namespace joda::processor
