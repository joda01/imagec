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

enum class Slot : int16_t
{
  $   = -1,
  $1  = 1,
  $2  = 2,
  $3  = 3,
  $4  = 4,
  $5  = 5,
  $6  = 6,
  $7  = 7,
  $8  = 8,
  $9  = 9,
  $10 = 10,
  $11 = 11,
  $12 = 12,
  $13 = 13,
  $14 = 14,
  $15 = 15,
  $16 = 16
};

NLOHMANN_JSON_SERIALIZE_ENUM(Slot, {{Slot::$, "$"},
                                    {Slot::$1, "$1"},
                                    {Slot::$2, "$2"},
                                    {Slot::$3, "$3"},
                                    {Slot::$4, "$4"},
                                    {Slot::$5, "$5"},
                                    {Slot::$6, "$6"},
                                    {Slot::$7, "$7"},
                                    {Slot::$8, "$8"},
                                    {Slot::$9, "$9"},
                                    {Slot::$10, "$10"},
                                    {Slot::$11, "$11"},
                                    {Slot::$12, "$12"},
                                    {Slot::$13, "$13"},
                                    {Slot::$14, "$14"},
                                    {Slot::$15, "$15"},
                                    {Slot::$16, "$16"}});

enum class ObjectClassId : int16_t
{

  NONE = -2,
  $    = -1,
  CL00 = 0,
  CL01 = 1,
  CL02 = 2,
  CL03 = 3,
  CL04 = 4,
  CL05 = 5,
  CL06 = 6,
  CL07 = 7,
  CL08 = 8,
  CL09 = 9,

};

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectClassId, {{ObjectClassId::NONE, "None"},
                                             {ObjectClassId::$, "$"},
                                             {ObjectClassId::CL00, "CL_00"},
                                             {ObjectClassId::CL01, "CL_01"},
                                             {ObjectClassId::CL02, "CL_02"},
                                             {ObjectClassId::CL03, "CL_03"},
                                             {ObjectClassId::CL04, "CL_04"},
                                             {ObjectClassId::CL05, "CL_05"},
                                             {ObjectClassId::CL06, "CL_06"},
                                             {ObjectClassId::CL07, "CL_07"},
                                             {ObjectClassId::CL08, "CL_08"},
                                             {ObjectClassId::CL09, "CL_09"}});

enum class ImageChannelIndex : int16_t
{
  NONE = -2,
  $    = -1,
  CH0  = 0,
  CH1  = 1,
  CH2  = 2,
  CH3  = 3,
  CH4  = 4,
  CH5  = 5,
  CH6  = 6,
  CH7  = 7,
  CH8  = 8,
  CH9  = 9
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

using tile_t   = std::tuple<int32_t, int32_t>;
using zStack_t = int32_t;
using tStack_t = int32_t;

}    // namespace joda::enums
