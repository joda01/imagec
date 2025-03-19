///
/// \file      pipeline_context.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <filesystem>
#include "backend/artifacts/image/image.hpp"
#include "backend/enums/enum_image_cache.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"

namespace joda::processor {

struct PipelineContext
{
  joda::atom::ImagePlane actImagePlane;
  enums::ClassId defaultClassId;
  enums::ZProjection defaultZProjection;
  int32_t pipelineIndex = 0;
  mutable enums::imageCache_t imageCache;
};

}    // namespace joda::processor
