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
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
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

void Processor::execute(const joda::settings::AnalyzeSettings &program)
{
  GlobalContext globalContext;
  globalContext.resultsOutputFolder = std::filesystem::path(program.projectSettings.workingDirectory);

  auto &db = globalContext.database;
  db.openDatabase(std::filesystem::path(program.projectSettings.workingDirectory) / "results.imcdb");
  auto jobId = db.startJob(program);

  //
  // Looking for images in all folders
  //
  std::map<uint8_t, std::unique_ptr<joda::filesystem::DirectoryWatcher>> allImages;
  for(const auto &plate : program.projectSettings.plates) {
    auto watcher = std::make_unique<joda::filesystem::DirectoryWatcher>();
    allImages.emplace(plate.plateId, std::move(watcher));
    allImages.at(plate.plateId)->setWorkingDirectory(plate.imageFolder);
    allImages.at(plate.plateId)->waitForFinished();
  }

  //
  // Iterate over each plate and analyze the images
  //
  for(const auto &plate : program.projectSettings.plates) {
    PlateContext plateContext{.plateId = plate.plateId};
    joda::grp::FileGrouper grouper(plate.groupBy, plate.filenameRegex);
    const auto &images = allImages.at(plate.plateId);

    //
    // Iterate over each image
    //
    const auto &fileList = images->getFilesList();
    for(const auto &imagePath : fileList) {
      PipelineInitializer imageLoader(program.projectSettings);
      ImageContext imageContext{.imageLoader = imageLoader};
      imageLoader.init(imagePath.getFilePath(), imageContext, globalContext);

      //
      // Assign image to group here!!
      //
      auto groupInfo = grouper.getGroupForFilename(imagePath.getFilePath());
      db.insertGroup(plateContext.plateId, groupInfo);
      db.insertImage(imageContext, groupInfo);
      db.insertImageChannels(imageContext.imageId, imageContext.imageMeta);
      db.insetImageToGroup(plateContext.plateId, imageContext.imageId, groupInfo.imageIdx, groupInfo);

      //
      // Start the iteration over planes
      //
      auto [tilesX, tilesY] = imageLoader.getNrOfTilesToProcess();
      auto nrtStack         = imageLoader.getNrOfTStacksToProcess();
      auto nrzSTack         = imageLoader.getNrOfZStacksToProcess();
      auto nrcSTack         = imageLoader.getNrOfCStacksToProcess();

      // Start of the image specific function
      for(int tStack = 0; tStack < nrtStack; tStack++) {
        for(int zStack = 0; zStack < nrzSTack; zStack++) {
          for(int cStack = 0; cStack < nrcSTack; cStack++) {
            IterationContext iterationContext;

            for(int tileX = 0; tileX < tilesX; tileX++) {
              for(int tileY = 0; tileY < tilesY; tileY++) {
                // Execute pipelines of this iteration
                for(const auto &pipeline : program.pipelines) {
                  //
                  // Load the image imagePlane
                  //
                  ProcessContext context{.globalContext    = globalContext,
                                         .plateContext     = plateContext,
                                         .imageContext     = imageContext,
                                         .iterationContext = iterationContext};
                  imageLoader.initPipeline(pipeline.pipelineSetup, {tilesX, tileY},
                                           {.tStack = tStack, .zStack = zStack, .cStack = cStack}, context);
                  auto planeId = context.getActImage().getId().imagePlane;
                  db.insertImagePlane(
                      imageContext.imageId, planeId,
                      imageContext.imageMeta.getChannelInfos().at(cStack).planes.at(planeId.tStack).at(planeId.zStack));

                  //
                  // Execute the pipeline
                  //
                  for(const auto &step : pipeline.pipelineSteps) {
                    // Execute a pipeline step
                    step(context, context.getActImage().image, context.getActObjects());
                  }
                }
              }
            }

            // Iteration for all tiles finished
            auto id = DurationCount::start("Insert");
            db.insertObjects(imageContext, iterationContext.getObjects());
            DurationCount::stop(id);
          }
          // End of the image specific function
        }
        // Image finished
      }
    }
  }

  // Done
  db.finishJob(jobId);
}
}    // namespace joda::processor
