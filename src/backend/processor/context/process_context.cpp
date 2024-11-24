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

ProcessContext::ProcessContext(GlobalContext &globalContext, PlateContext &plateContext, ImageContext &imageContext,
                               IterationContext &iterationContext) :
    globalContext(globalContext),
    plateContext(plateContext), imageContext(imageContext), iterationContext(iterationContext)
{
}

[[nodiscard]] const joda::atom::ImagePlane *ProcessContext::loadImageFromCache(joda::enums::ImageId cacheId)
{
  getCorrectIteration(cacheId.imagePlane);
  if(!doesImageInCacheExist(cacheId)) {
    // Load image to cache
    if(cacheId.zProjection == enums::ZProjection::UNDEFINED) {
      throw std::invalid_argument("Define image plane to load from!");
    }

    if(cacheId.zProjection == enums::ZProjection::$) {
      cacheId.zProjection = pipelineContext.actImagePlane.getId().zProjection;
    }

    imageContext.imageLoader.loadImageToCache(cacheId.imagePlane, cacheId.zProjection, pipelineContext.actImagePlane.tile, *this);
  }

  return iterationContext.imageCache.at(cacheId).get();
}

std::string ProcessContext::getColorForClusterAndClass(enums::ClusterIdIn cluster, enums::ClassIdIn classs) const
{
  settings::ClassificatorSettingOut tmp{getClusterId(cluster), getClassId(classs)};
  auto color = globalContext.clusterClassColors.getColor(tmp);
  if(color.empty()) {
    // Assign a random color if no color was found
  }
  return color;
}

}    // namespace joda::processor
