

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
      return "none";
    case ChannelIndex::CH0:
      return "ch_00";
    case ChannelIndex::CH1:
      return "ch_01";
    case ChannelIndex::CH2:
      return "ch_02";
    case ChannelIndex::CH3:
      return "ch_03";
    case ChannelIndex::CH4:
      return "ch_04";
    case ChannelIndex::CH5:
      return "ch_05";
    case ChannelIndex::CH6:
      return "ch_06";
    case ChannelIndex::CH7:
      return "ch_07";
    case ChannelIndex::CH8:
      return "ch_08";
    case ChannelIndex::CH9:
      return "ch_09";
    case ChannelIndex::A:
      return "slot_a";
    case ChannelIndex::B:
      return "slot_b";
    case ChannelIndex::C:
      return "slot_c";
    case ChannelIndex::D:
      return "slot_d";
    case ChannelIndex::E:
      return "slot_e";
    case ChannelIndex::F:
      return "slot_f";
  }

  return "ups";
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
