///
/// \file      process_context.cpp
/// \author
/// \date      2024-08-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "process_context.hpp"
#include <stdexcept>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"

namespace joda::processor {

ProcessContext::ProcessContext(GlobalContext &globalContext, PlateContext &plateContext, ImageContext &imageContext,
                               IterationContext &iterationContext) :
    globalContext(globalContext),
    plateContext(plateContext), imageContext(imageContext), iterationContext(iterationContext)
{
}

[[nodiscard]] const joda::atom::ImagePlane *ProcessContext::loadImageFromCache(joda::enums::ImageId cacheId)
{
  getCorrectIteration(cacheId.imagePlane);

  if(cacheId.memoryId != enums::MemoryIdx::NONE) {
    if(!doesImageInCacheExist(cacheId)) {
      throw std::invalid_argument("Image does not exist in cache!");
    }
  } else if(!doesImageInCacheExist(cacheId)) {
    // Load image to cache
    if(cacheId.zProjection == enums::ZProjection::UNDEFINED) {
      throw std::invalid_argument("Define image plane to load from!");
    }

    if(cacheId.zProjection == enums::ZProjection::$) {
      cacheId.zProjection = pipelineContext.actImagePlane.getId().zProjection;
    }

    joda::processor::PipelineInitializer::loadImageAndStoreToCache(cacheId.imagePlane, cacheId.zProjection, pipelineContext.actImagePlane.tile, *this,
                                                                   imageContext);
  }

  return iterationContext.imageCache.at(getMemoryIdx(cacheId)).get();
}

}    // namespace joda::processor
