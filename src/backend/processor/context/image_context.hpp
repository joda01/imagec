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

using imageCache_t = std::map<enums::ImageId, std::unique_ptr<joda::atom::ImagePlane>>;

struct ImageContext
{
  std::filesystem::path imagePath;
  joda::ome::OmeInfo imageMeta;
  cv::Size tileSize;
  uint64_t imageId;
  imageCache_t imageCache;
};

}    // namespace joda::processor
