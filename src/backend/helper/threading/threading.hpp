///
/// \file      threading.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <cstdint>
#include <map>

namespace joda::thread {

struct ThreadingSettings
{
  ThreadingSettings()
  {
  }
  enum Type
  {
    IMAGES,
    TILES,
    CHANNELS
  };
  uint64_t ramPerImage    = 0;
  uint64_t ramFree        = 0;
  uint64_t ramTotal       = 0;
  uint32_t coresAvailable = 0;
  uint32_t coresUsed      = 0;
  uint64_t totalRuns      = 0;
  std::map<Type, int32_t> cores{{IMAGES, 1}, {TILES, 1}, {CHANNELS, 1}};
};
}    // namespace joda::thread
