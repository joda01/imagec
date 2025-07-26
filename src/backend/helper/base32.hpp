///
/// \file      base32.hpp
/// \author    Joachim Danmayr
/// \date      2025-06-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <string>

namespace joda::helper {
inline std::string toBase32(uint64_t number)
{
  const std::string alphabet = "ABCDEFGHJKMNPQRSTUVWXYZ123456789";
  std::string result;

  // Convert number to base32 string (no padding yet)
  while(number > 0) {
    int remainder = number % 32;
    number /= 32;
    result.insert(result.begin(), alphabet[remainder]);
  }

  if(result.empty()) {
    result = alphabet[0];    // zero
  }

  // Calculate padding length to nearest multiple of 3
  size_t len        = result.length();
  size_t padded_len = ((len + 2) / 3) * 3;    // next multiple of 3

  // Pad with 'A' (which means 0) to left to reach padded_len
  while(result.length() < padded_len) {
    result.insert(result.begin(), alphabet[0]);
  }

  // Insert '-' after every 3 digits
  for(size_t i = 3; i < result.length(); i += 4) {
    result.insert(i, "-");
  }

  return result;
}
}    // namespace joda::helper
