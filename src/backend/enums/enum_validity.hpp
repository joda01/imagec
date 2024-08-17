///
/// \file      enum_validity.hpp
/// \author
/// \date      2024-08-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <bitset>
#include <cstddef>
#include <cstdint>

namespace joda::enums {

enum class ChannelValidityEnum : size_t
{
  UNKNOWN                  = 1,
  INVALID                  = 2,
  MANUAL_OUT_SORTED        = 3,
  POSSIBLE_NOISE           = 4,
  POSSIBLE_WRONG_THRESHOLD = 5
};

struct ChannelValidity : public std::bitset<32>
{
  using bitset<32>::bitset;

  bool test(ChannelValidityEnum idx)
  {
    return bitset<32>::test(static_cast<size_t>(idx));
  }

  void set(ChannelValidityEnum idx)
  {
    bitset<32>::set(static_cast<size_t>(idx));
  }
};
}    // namespace joda::enums
