
///
/// \file      fnv1a.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <cstdint>
#include <string>

inline constexpr uint64_t FNV_PRIME        = 1099511628211ULL;
inline constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;

inline uint64_t fnv1a(const std::string &str)
{
  uint64_t hash = FNV_OFFSET_BASIS;
  for(char c : str) {
    hash ^= static_cast<uint64_t>(c);
    hash *= FNV_PRIME;
  }
  return hash;
}
