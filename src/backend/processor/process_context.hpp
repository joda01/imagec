
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
#include "backend/commands/functions/image_loader/channel_loader_settings.hpp"
#include "backend/settings/anaylze_settings_enums.hpp"

namespace joda::processor {

struct ProcessContext
{
  std::filesystem::path imagePath;
  std::filesystem::path resultsOutputFolder;
  joda::settings::tile_t tile     = {0, 0};
  joda::settings::tStack_t tStack = 0;
  joda::settings::zStack_t zStack = 0;
  joda::settings::ImageChannelIndex channel;
  joda::cmd::functions::ChannelLoaderSettings loader;
};

}    // namespace joda::processor