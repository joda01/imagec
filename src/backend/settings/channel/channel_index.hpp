

#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>

namespace joda::settings {

enum class ChannelIndex : int32_t
{
  NONE = -1,
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
  A    = 65,
  B    = 66,
  C    = 67,
  D    = 68,
  E    = 69,
  F    = 70
};

inline std::string to_string(joda::settings::ChannelIndex idx)
{
  switch(idx) {
    case ChannelIndex::NONE:
    case ChannelIndex::CH0:
    case ChannelIndex::CH1:
    case ChannelIndex::CH2:
    case ChannelIndex::CH3:
    case ChannelIndex::CH4:
    case ChannelIndex::CH5:
    case ChannelIndex::CH6:
    case ChannelIndex::CH7:
    case ChannelIndex::CH8:
    case ChannelIndex::CH9:
    case ChannelIndex::A:
    case ChannelIndex::B:
    case ChannelIndex::C:
    case ChannelIndex::D:
    case ChannelIndex::E:
    case ChannelIndex::F:
      break;
  }

  return "";
}

NLOHMANN_JSON_SERIALIZE_ENUM(ChannelIndex, {
                                               {ChannelIndex::NONE, "None"},
                                               {ChannelIndex::CH0, "0"},
                                               {ChannelIndex::CH1, "1"},
                                               {ChannelIndex::CH2, "2"},
                                               {ChannelIndex::CH3, "3"},
                                               {ChannelIndex::CH4, "4"},
                                               {ChannelIndex::CH5, "5"},
                                               {ChannelIndex::CH6, "6"},
                                               {ChannelIndex::CH7, "7"},
                                               {ChannelIndex::CH8, "8"},
                                               {ChannelIndex::CH9, "9"},
                                               {ChannelIndex::A, "A"},
                                               {ChannelIndex::B, "B"},
                                               {ChannelIndex::C, "C"},
                                               {ChannelIndex::D, "D"},
                                               {ChannelIndex::E, "E"},
                                               {ChannelIndex::F, "F"},
                                           });

}    // namespace joda::settings
