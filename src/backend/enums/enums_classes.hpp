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

enum class ClassId : uint16_t
{
  C0   = 0,
  C1   = 1,
  C2   = 2,
  C3   = 3,
  C4   = 4,
  C5   = 5,
  C6   = 6,
  C7   = 7,
  C8   = 8,
  C9   = 9,
  C10  = 10,
  C11  = 11,
  C12  = 12,
  C13  = 13,
  C14  = 14,
  C15  = 15,
  C16  = 16,
  NONE = 0xFFFE,
};

enum class ClassIdIn : uint16_t
{
  C0   = 0,
  C1   = 1,
  C2   = 2,
  C3   = 3,
  C4   = 4,
  C5   = 5,
  C6   = 6,
  C7   = 7,
  C8   = 8,
  C9   = 9,
  C10  = 10,
  C11  = 11,
  C12  = 12,
  C13  = 13,
  C14  = 14,
  C15  = 15,
  C16  = 16,
  NONE = 0xFFFE,
  $    = 0xFFFF
};

NLOHMANN_JSON_SERIALIZE_ENUM(ClassIdIn, {{ClassIdIn::NONE, "None"},
                                         {ClassIdIn::$, "$"},
                                         {ClassIdIn::C0, "C0"},
                                         {ClassIdIn::C1, "C1"},
                                         {ClassIdIn::C2, "C2"},
                                         {ClassIdIn::C3, "C3"},
                                         {ClassIdIn::C4, "C4"},
                                         {ClassIdIn::C5, "C5"},
                                         {ClassIdIn::C6, "C6"},
                                         {ClassIdIn::C7, "C7"},
                                         {ClassIdIn::C8, "C8"},
                                         {ClassIdIn::C9, "C9"},
                                         {ClassIdIn::C10, "C10"},
                                         {ClassIdIn::C11, "C11"},
                                         {ClassIdIn::C12, "C12"},
                                         {ClassIdIn::C13, "C13"},
                                         {ClassIdIn::C14, "C14"},
                                         {ClassIdIn::C15, "C15"},
                                         {ClassIdIn::C16, "C16"}});

NLOHMANN_JSON_SERIALIZE_ENUM(ClassId, {{ClassId::NONE, "None"},
                                       {ClassId::C0, "C0"},
                                       {ClassId::C1, "C1"},
                                       {ClassId::C2, "C2"},
                                       {ClassId::C3, "C3"},
                                       {ClassId::C4, "C4"},
                                       {ClassId::C5, "C5"},
                                       {ClassId::C6, "C6"},
                                       {ClassId::C7, "C7"},
                                       {ClassId::C8, "C8"},
                                       {ClassId::C9, "C9"},
                                       {ClassId::C10, "C10"},
                                       {ClassId::C11, "C11"},
                                       {ClassId::C12, "C12"},
                                       {ClassId::C13, "C13"},
                                       {ClassId::C14, "C14"},
                                       {ClassId::C15, "C15"},
                                       {ClassId::C16, "C16"}});

}    // namespace joda::enums
