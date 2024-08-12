///
/// \file      pipeline_indexes.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <nlohmann/json.hpp>

namespace joda::enums {

enum class ChannelId : uint16_t
{
  A    = 1,
  B    = 2,
  C    = 3,
  D    = 4,
  E    = 5,
  F    = 6,
  G    = 7,
  H    = 8,
  I    = 9,
  J    = 10,
  NONE = 0xFFFE,
  $    = 0xFFFF
};

inline int32_t toInt(ChannelId idx)
{
  return static_cast<int32_t>(idx);
}

NLOHMANN_JSON_SERIALIZE_ENUM(ChannelId, {
                                            {ChannelId::NONE, "None"},
                                            {ChannelId::$, "$"},
                                            {ChannelId::A, "A"},
                                            {ChannelId::B, "B"},
                                            {ChannelId::C, "C"},
                                            {ChannelId::D, "D"},
                                            {ChannelId::E, "E"},
                                            {ChannelId::F, "F"},
                                            {ChannelId::G, "G"},
                                            {ChannelId::H, "H"},
                                            {ChannelId::I, "I"},
                                            {ChannelId::J, "J"},
                                        });

}    // namespace joda::enums
