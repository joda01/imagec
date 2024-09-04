///
/// \file      image_context.hpp
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
#include "backend/enums/enum_images.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"

namespace joda::processor {
class PipelineInitializer;
}

namespace joda::processor {

struct ImageContext
{
  PipelineInitializer &imageLoader;
  std::filesystem::path imagePath;
  const joda::ome::OmeInfo &imageMeta;
  uint64_t imageId;
  cv::Size tileSize;
};

}    // namespace joda::processor
