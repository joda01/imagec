
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
/// \brief     A short description what happens here.
///

#pragma once

#include <filesystem>
#include <memory>
#include "backend/commands/image_functions/image_loader/channel_loader_settings.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"

namespace joda::processor {

struct ProcessContext
{
  // Global parameters
  std::filesystem::path resultsOutputFolder;
  std::shared_ptr<joda::db::Database> database;

  // Actual processed image part
  std::filesystem::path imagePath;
  joda::enums::tile_t tile     = {0, 0};
  joda::enums::tStack_t tStack = 0;
  joda::enums::zStack_t zStack = 0;
  joda::enums::ImageChannelIndex cStack;
  std::optional<joda::ome::OmeInfo> imageMeta;

  // Image loader settings
  joda::cmd::functions::ChannelLoaderSettings loader;
  cv::Mat originalImage;

  // Data modified from commands during pipeline run
  bool isBinary                = false;
  uint16_t appliedMinThreshold = 0;
  uint16_t appliedMaxThreshold = 0;
};

}    // namespace joda::processor
