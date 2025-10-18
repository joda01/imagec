
///
/// \file      fnv1a.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-03
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
#include <filesystem>
#include <string>

namespace joda::helper {

inline constexpr uint64_t FNV_PRIME        = 1099511628211ULL;
inline constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;

inline uint64_t calcFnv1a(const std::string &str)
{
  uint64_t hash = FNV_OFFSET_BASIS;
  for(char c : str) {
    hash ^= static_cast<uint64_t>(c);
    hash *= FNV_PRIME;
  }
  return hash;
}

inline uint64_t generateImageIdFromPath(const std::filesystem::path &imageFilePath, const std::filesystem::path &workingDirectory)
{
  auto relativePath = std::filesystem::relative(imageFilePath, workingDirectory);
  return calcFnv1a(relativePath.string());
}

}    // namespace joda::helper
