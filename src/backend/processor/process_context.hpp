
///
/// \file      process_step.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <filesystem>
#include <memory>
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::processor {

struct GlobalContext
{
  std::filesystem::path resultsOutputFolder;
  // std::shared_ptr<joda::db::Database> database;
};

struct ImageContext
{
  std::filesystem::path imagePath;
  joda::ome::OmeInfo imageMeta;
};

struct ImagePipelineContext
{
  // Actual processed image part
  cv::Mat originalImage;
  joda::enums::tile_t tile     = {0, 0};
  joda::enums::tStack_t tStack = 0;
  joda::enums::zStack_t zStack = 0;
  joda::enums::cStack_t cStack = 0;
};

struct ImageProcessingContext
{
  bool isBinary                = false;
  uint16_t appliedMinThreshold = 0;
  uint16_t appliedMaxThreshold = 0;
};

struct ProcessContext
{
  GlobalContext &globalContext;
  ImageContext &imageContext;
  ImagePipelineContext imagePipelineContext;
  ImageProcessingContext imageProcessingContext;
};

}    // namespace joda::processor
