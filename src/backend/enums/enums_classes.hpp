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
enum class ClassIdIn : uint16_t
{
  C0        = 0,
  C1        = 1,
  C2        = 2,
  C3        = 3,
  C4        = 4,
  C5        = 5,
  C6        = 6,
  C7        = 7,
  C8        = 8,
  C9        = 9,
  C10       = 10,
  C11       = 11,
  C12       = 12,
  C13       = 13,
  C14       = 14,
  C15       = 15,
  C16       = 16,
  NONE      = 0xFFFD,
  UNDEFINED = 0xFFFE,
  $         = 0xFFFF
};

enum class ClassId : uint16_t
{
  C0        = 0,
  C1        = 1,
  C2        = 2,
  C3        = 3,
  C4        = 4,
  C5        = 5,
  C6        = 6,
  C7        = 7,
  C8        = 8,
  C9        = 9,
  C10       = 10,
  C11       = 11,
  C12       = 12,
  C13       = 13,
  C14       = 14,
  C15       = 15,
  C16       = 16,
  NONE      = 0xFFFD,
  UNDEFINED = 0xFFFE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ClassId, {{ClassId::NONE, "None"},
                                       {ClassId::UNDEFINED, "Undefined"},
                                       {ClassId::C0, "0"},
                                       {ClassId::C1, "1"},
                                       {ClassId::C2, "2"},
                                       {ClassId::C3, "3"},
                                       {ClassId::C4, "4"},
                                       {ClassId::C5, "5"},
                                       {ClassId::C6, "6"},
                                       {ClassId::C7, "7"},
                                       {ClassId::C8, "8"},
                                       {ClassId::C9, "9"},
                                       {ClassId::C10, "10"},
                                       {ClassId::C11, "11"},
                                       {ClassId::C12, "12"},
                                       {ClassId::C13, "13"},
                                       {ClassId::C14, "14"},
                                       {ClassId::C15, "15"},
                                       {ClassId::C16, "16"}});

NLOHMANN_JSON_SERIALIZE_ENUM(ClassIdIn, {{ClassIdIn::NONE, "None"}, {ClassIdIn::UNDEFINED, "Undefined"},
                                         {ClassIdIn::$, "$"},       {ClassIdIn::C0, "0"},
                                         {ClassIdIn::C1, "1"},      {ClassIdIn::C2, "2"},
                                         {ClassIdIn::C3, "3"},      {ClassIdIn::C4, "4"},
                                         {ClassIdIn::C5, "5"},      {ClassIdIn::C6, "6"},
                                         {ClassIdIn::C7, "7"},      {ClassIdIn::C8, "8"},
                                         {ClassIdIn::C9, "9"},      {ClassIdIn::C10, "10"},
                                         {ClassIdIn::C11, "11"},    {ClassIdIn::C12, "12"},
                                         {ClassIdIn::C13, "13"},    {ClassIdIn::C14, "14"},
                                         {ClassIdIn::C15, "15"},    {ClassIdIn::C16, "16"}});

}    // namespace joda::enums
