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
#include "backend/helper/directory_iterator.hpp"
#include "backend/helper/file_info_images.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/processor/process_context.hpp"
#include "backend/processor/processor_memory.hpp"

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
  images.setWorkingDirectory(program.images.imageInputDirectory);
  images.waitForFinished();

  GlobalContext globalContext;

  for(const auto &imagePath : images.getFilesList()) {
    ImageContext imageContext;
    PipelineInitializer imageLoader(program.images, imagePath.getFilePath(), imageContext);

    auto [tilesX, tilesY] = imageLoader.getNrOfTilesToProcess();
    auto nrtStack         = imageLoader.getNrOfTStacksToProcess();
    auto nrzSTack         = imageLoader.getNrOfZStacksToProcess();
    auto nrcSTack         = imageLoader.getNrOfCStacksToProcess();

    for(int tileX = 0; tileX < tilesX; tileX++) {
      for(int tileY = 0; tileY < tilesY; tileY++) {
        for(int tStack = 0; tStack < nrtStack; tStack++) {
          for(int zStack = 0; zStack < nrzSTack; zStack++) {
            for(int cStack = 0; cStack < nrcSTack; cStack++) {
              // Start pipeline
              for(const auto &pipeline : program.pipelines) {
                ProcessContext context{.globalContext = globalContext, .imageContext = imageContext};
                ProcessStep processStep(context);
                imageLoader.load(pipeline.inputImage,
                                 PipelineInitializer::PartToLoad{
                                     .tile = {tilesX, tileY}, .tStack = tStack, .zStack = zStack, .cStack = cStack},
                                 processStep);
                for(const auto &step : pipeline.pipelineSteps) {
                  processStep.executeStep(mMemory, step);
                }
              }

              // Image section finished
              // Do cross channel measurement here
            }
          }
        }
      }
      // Image finished
    }
  }
}
}    // namespace joda::processor
