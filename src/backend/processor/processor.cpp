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
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/directory_iterator.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_info_images.hpp"
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
  joda::helper::fs::DirectoryWatcher<helper::fs::FileInfoImages> images({});
  images.setWorkingDirectory(program.projectSettings.imageSetup.imageInputDirectory);
  images.waitForFinished();

  GlobalContext globalContext;
  auto &db = globalContext.database;
  db.openDatabase(std::filesystem::path(program.projectSettings.outputDirectory) / "results.imcdb");
  db.insertProjectSettings(program);

  for(const auto &imagePath : images.getFilesList()) {
    ImageContext imageContext;
    PipelineInitializer imageLoader(program.projectSettings, imagePath.getFilePath(), imageContext, globalContext);
    db.insertImage(imageContext);

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
                ProcessContext context{
                    .globalContext = globalContext, .imageContext = imageContext, .iterationContext = iterationContext};
                imageLoader.initPipeline(pipeline.pipelineSetup, {tilesX, tileY},
                                         joda::enums::PlaneId{.tStack = tStack, .zStack = zStack, .cStack = cStack},
                                         context);
                db.insertImagePlane();

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
}    // namespace joda::processor
