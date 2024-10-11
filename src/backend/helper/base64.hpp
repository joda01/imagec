///
/// \file      base64.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <string>
#include <vector>

namespace joda::helper {

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

inline std::string base64Encode(const std::string &in)
{
  std::string out;
  int val  = 0;
  int valb = -6;
  for(unsigned char c : in) {
    val = (val << 8) + c;
    valb += 8;
    while(valb >= 0) {
      out.push_back(base64_chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if(valb > -6) {
    out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }
  while(out.size() % 4) {
    out.push_back('=');
  }
  return out;
}

inline std::string base64Decode(const std::string &in)
{
  std::string out;
  std::vector<int> T(256, -1);
  for(int i = 0; i < 64; i++) {
    T[base64_chars[i]] = i;
  }

  int val  = 0;
  int valb = -8;
  for(unsigned char c : in) {
    if(T[c] == -1) {
      break;    // Ignore non-base64 characters
    }
    val = (val << 6) + T[c];
    valb += 6;
    if(valb >= 0) {
      out.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}

}    // namespace joda::helper
