#pragma once

#include <string>

#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

namespace joda::helper {

inline std::string generate_uuid()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dis;

  std::stringstream ss;
  ss << std::hex << std::setfill('0');

  for(int i = 0; i < 8; ++i) {
    ss << std::setw(2) << (dis(gen) & 0xff);
  }
  ss << '-';
  for(int i = 0; i < 4; ++i) {
    ss << std::setw(2) << (dis(gen) & 0xff);
  }
  ss << '-';
  for(int i = 0; i < 4; ++i) {
    ss << std::setw(2) << (dis(gen) & 0xff);
  }
  ss << '-';
  for(int i = 0; i < 4; ++i) {
    ss << std::setw(2) << (dis(gen) & 0xff);
  }
  ss << '-';
  for(int i = 0; i < 12; ++i) {
    ss << std::setw(2) << (dis(gen) & 0xff);
  }

  return ss.str();
}

}    // namespace joda::helper
