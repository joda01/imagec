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
/// \brief     A short description what happens here.
///

#include "processor.hpp"
#include <filesystem>
#include "backend/commands/functions/image_loader/image_loader.hpp"
#include "backend/helper/directory_iterator.hpp"
#include "backend/helper/file_info_images.hpp"
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

  for(const auto &imagePath : images.getFilesList()) {
    joda::cmd::functions::ImageLoader imageLoader(program.imageLoader, imagePath.getFilePath());

    auto [tilesX, tilesY] = imageLoader.getNrOfTilesToProcess();
    auto nrtStack         = imageLoader.getNrOfTStacksToProcess();
    auto nrzSTack         = imageLoader.getNrOfZStacksToProcess();

    for(int tileX = 0; tileX < tilesX; tileX++) {
      for(int tileY = 0; tileY < tilesY; tileY++) {
        for(int tStack = 0; tStack < nrtStack; tStack++) {
          for(int zStack = 0; zStack < nrzSTack; zStack++) {
            // Start pipeline
            for(const auto &pipeline : program.pipelines) {
              // First of all load the image to process
              ProcessStep actStep{ProcessContext{.imagePath = imagePath.getFilePath(),
                                                 .tile      = {tileX, tileY},
                                                 .tStack    = tStack,
                                                 .zStack    = zStack,
                                                 .channel   = joda::settings::ImageChannelIndex::NONE,
                                                 .loader    = pipeline.channelLoader}};

              actStep.executeStep(mMemory, imageLoader);
              for(const auto &step : pipeline.pipelineSteps) {
                ProcessStep &stepToUseForProcessing = actStep;
                if(step.input != joda::settings::Slot::$) {
                  mMemory.loadCopy(step.input, actStep);
                }
                actStep.executeStep(mMemory, step);
              }
            }
            // Image section finished
          }
        }
      }
    }
    // Image finished
  }
}

}    // namespace joda::processor
