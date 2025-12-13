///
/// \file      threading.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
    CHANNELS,
    STACKS
  };
  uint64_t ramPerImage    = 0;
  uint64_t ramFree        = 0;
  uint64_t ramTotal       = 0;
  uint32_t coresAvailable = 0;
  uint32_t coresUsed      = 0;
  uint64_t totalRuns      = 0;
  std::map<Type, int32_t> cores{{IMAGES, 1}, {TILES, 1}, {CHANNELS, 1}, {STACKS, 1}};
};
}    // namespace joda::thread
