///
/// \file      pipeline_context.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <filesystem>
#include "backend/artifacts/image/image.hpp"
#include "backend/enums/enum_image_cache.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_units.hpp"

namespace joda::processor {

struct PipelineContext
{
  joda::atom::ImagePlane actImagePlane;
  enums::ClassId defaultClassId;
  enums::ZProjection defaultZProjection;
  int32_t pipelineIndex = 0;
  mutable enums::imageCache_t imageCache;
  enums::Units pixelSizeUnit = enums::Units::um;
};

}    // namespace joda::processor
