///
/// \file      image_context.hpp
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
  uint32_t nrOfZStacks  = 0;
  int32_t series        = 0;
  bool loadImageInTiles = false;
};

}    // namespace joda::processor
