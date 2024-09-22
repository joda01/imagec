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

enum class ClusterIdIn : uint16_t
{
  A         = 0,
  B         = 1,
  C         = 2,
  D         = 3,
  E         = 4,
  F         = 5,
  G         = 6,
  H         = 7,
  I         = 8,
  J         = 9,
  K         = 10,
  L         = 11,
  M         = 12,
  N         = 13,
  O         = 14,
  P         = 15,
  Q         = 16,
  NONE      = 0xFFFD,
  UNDEFINED = 0xFFFE,
  $         = 0xFFFF
};

enum class ClusterId : uint16_t
{
  A         = 0,
  B         = 1,
  C         = 2,
  D         = 3,
  E         = 4,
  F         = 5,
  G         = 6,
  H         = 7,
  I         = 8,
  J         = 9,
  K         = 10,
  L         = 11,
  M         = 12,
  N         = 13,
  O         = 14,
  P         = 15,
  Q         = 16,
  NONE      = 0xFFFD,
  UNDEFINED = 0xFFFE,
};

inline int32_t toInt(ClusterId idx)
{
  return static_cast<int32_t>(idx);
}

NLOHMANN_JSON_SERIALIZE_ENUM(ClusterIdIn, {
                                              {ClusterIdIn::NONE, "None"}, {ClusterIdIn::UNDEFINED, "Undefined"},
                                              {ClusterIdIn::$, "$"},       {ClusterIdIn::A, "A"},
                                              {ClusterIdIn::B, "B"},       {ClusterIdIn::C, "C"},
                                              {ClusterIdIn::D, "D"},       {ClusterIdIn::E, "E"},
                                              {ClusterIdIn::F, "F"},       {ClusterIdIn::G, "G"},
                                              {ClusterIdIn::H, "H"},       {ClusterIdIn::I, "I"},
                                              {ClusterIdIn::J, "J"},       {ClusterIdIn::K, "K"},
                                              {ClusterIdIn::L, "L"},       {ClusterIdIn::M, "M"},
                                              {ClusterIdIn::N, "N"},       {ClusterIdIn::O, "O"},
                                              {ClusterIdIn::P, "P"},       {ClusterIdIn::Q, "Q"},
                                          });

NLOHMANN_JSON_SERIALIZE_ENUM(ClusterId, {
                                            {ClusterId::NONE, "None"}, {ClusterId::UNDEFINED, "Undefined"},
                                            {ClusterId::A, "A"},       {ClusterId::B, "B"},
                                            {ClusterId::C, "C"},       {ClusterId::D, "D"},
                                            {ClusterId::E, "E"},       {ClusterId::F, "F"},
                                            {ClusterId::G, "G"},       {ClusterId::H, "H"},
                                            {ClusterId::I, "I"},       {ClusterId::J, "J"},
                                            {ClusterId::K, "K"},       {ClusterId::L, "L"},
                                            {ClusterId::M, "M"},       {ClusterId::N, "N"},
                                            {ClusterId::O, "O"},       {ClusterId::P, "P"},
                                            {ClusterId::Q, "Q"},
                                        });

}    // namespace joda::enums
