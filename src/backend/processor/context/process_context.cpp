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
#include "backend/database/database.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"

namespace joda::processor {

ProcessContext::ProcessContext(const GlobalContext &globalContextIn, const PipelineInitializer &imageContextIn,
                               IterationContext &iterationContextIn) :
    globalContext(globalContextIn),
    imageContext(imageContextIn), iterationContext(iterationContextIn)
{
}

[[nodiscard]] const joda::atom::ImagePlane *ProcessContext::loadImageFromCache(enums::MemoryScope scope, joda::enums::ImageId cacheId)
{
  getCorrectIteration(cacheId.imagePlane);

  if(cacheId.memoryId != enums::MemoryIdx::NONE) {
    if(!doesImageInCacheExist(scope, cacheId)) {
      throw std::invalid_argument("Image does not exist in cache!");
    }
  } else if(!doesImageInCacheExist(scope, cacheId)) {
    // Load image to cache
    if(cacheId.zProjection == enums::ZProjection::UNDEFINED) {
      throw std::invalid_argument("Define image plane to load from!");
    }

    if(cacheId.zProjection == enums::ZProjection::$) {
      cacheId.zProjection       = pipelineContext.actImagePlane.getId().zProjection;
      cacheId.imagePlane.zStack = getActIterator().zStack;
    }
    if(cacheId.imagePlane.zStack < 0) {
      cacheId.imagePlane.zStack = getActIterator().zStack;
    }

    if(cacheId.imagePlane.tStack < 0) {
      cacheId.imagePlane.tStack = getActIterator().tStack;
    }

    imageContext.loadImageAndStoreToCache(scope, cacheId.imagePlane, cacheId.zProjection, pipelineContext.actImagePlane.tile, *this);
  }
  if(scope == enums::MemoryScope::ITERATION) {
    return iterationContext.imageCache.at(getMemoryIdx(cacheId)).get();
  } else {
    return pipelineContext.imageCache.at(getMemoryIdx(cacheId)).get();
  }
}

[[nodiscard]] const std::filesystem::path &ProcessContext::getActImagePath() const
{
  return imageContext.getImagePath();
}

void ProcessContext::setImageValidity(enums::ChannelValidityEnum validityIn)
{
  enums::ChannelValidity validity;
  validity.set(validityIn);
  globalContext.database->setImageValidity(imageContext.getImageId(), validity);
}

void ProcessContext::setImagePlaneValidity(enums::ChannelValidityEnum validityIn)
{
  enums::ChannelValidity validity;
  validity.set(validityIn);
  globalContext.database->setImagePlaneValidity(imageContext.getImageId(), getActIterator(), validity);
}

void ProcessContext::setImagePlaneClasssClasssValidity(enums::ClassIdIn classIn, enums::ChannelValidityEnum validityIn)
{
  enums::ChannelValidity validity;
  validity.set(validityIn);
  globalContext.database->setImagePlaneClasssClasssValidity(imageContext.getImageId(), getActIterator(), getClassId(classIn), validity);
}

[[nodiscard]] cv::Size ProcessContext::getOriginalImageSize() const
{
  return {imageContext.getImageWidth(), imageContext.getImageHeight()};
}

[[nodiscard]] cv::Size ProcessContext::getTileSize() const
{
  return imageContext.getTileSize();
}

ome::PhyiscalSize ProcessContext::getPhysicalPixelSIzeOfImage() const
{
  return imageContext.getPhysicalPixelSIzeOfImage();
}

}    // namespace joda::processor
