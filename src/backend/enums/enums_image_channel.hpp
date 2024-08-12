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
/// \brief     A short description what happens here.
///

#pragma once

#include <nlohmann/json.hpp>

namespace joda::enums {

enum class ImageChannelIndex : uint16_t
{
  CH0  = 0,
  CH1  = 1,
  CH2  = 2,
  CH3  = 3,
  CH4  = 4,
  CH5  = 5,
  CH6  = 6,
  CH7  = 7,
  CH8  = 8,
  CH9  = 9,
  NONE = 0xFFFE,
  $    = 0xFFFF,    // This is the this (me channel)
};

inline int32_t toInt(ImageChannelIndex idx)
{
  return static_cast<int32_t>(idx);
}

NLOHMANN_JSON_SERIALIZE_ENUM(ImageChannelIndex, {
                                                    {ImageChannelIndex::NONE, "None"},
                                                    {ImageChannelIndex::$, "$"},
                                                    {ImageChannelIndex::CH0, "CH_00"},
                                                    {ImageChannelIndex::CH1, "CH_01"},
                                                    {ImageChannelIndex::CH2, "CH_02"},
                                                    {ImageChannelIndex::CH3, "CH_03"},
                                                    {ImageChannelIndex::CH4, "CH_04"},
                                                    {ImageChannelIndex::CH5, "CH_05"},
                                                    {ImageChannelIndex::CH6, "CH_06"},
                                                    {ImageChannelIndex::CH7, "CH_07"},
                                                    {ImageChannelIndex::CH8, "CH_08"},
                                                });

}    // namespace joda::enums
