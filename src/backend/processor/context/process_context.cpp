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
#include "backend/processor/initializer/pipeline_initializer.hpp"

namespace joda::processor {

[[nodiscard]] const joda::atom::ImagePlane *ProcessContext::loadImageFromCache(joda::enums::ImageId cacheId)
{
  getCorrectIteration(cacheId.imagePlane);
  if(!doesImageInCacheExist(cacheId)) {
    // Load image to cache
    if(cacheId.imageIdx == enums::ZProjection::UNDEFINED) {
      throw std::invalid_argument("Define image plane to load from!");
    }

    if(cacheId.imageIdx == enums::ZProjection::$) {
      cacheId.imageIdx = pipelineContext.actImagePlane.getId().imageIdx;
    }

    imageContext.imageLoader.loadImageToCache(cacheId.imagePlane, cacheId.imageIdx, pipelineContext.actImagePlane.tile,
                                              *this);
  }

  return imageContext.imageCache.at(cacheId).get();
}

}    // namespace joda::processor
