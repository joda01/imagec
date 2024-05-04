

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

inline ChannelIndex from_string(const std::string &idx)
{
  if(idx == "None") {
    return ChannelIndex::NONE;
  }
  if(idx == "0") {
    return ChannelIndex::CH0;
  }
  if(idx == "1") {
    return ChannelIndex::CH1;
  }
  if(idx == "2") {
    return ChannelIndex::CH2;
  }
  if(idx == "3") {
    return ChannelIndex::CH3;
  }
  if(idx == "4") {
    return ChannelIndex::CH4;
  }
  if(idx == "5") {
    return ChannelIndex::CH5;
  }
  if(idx == "6") {
    return ChannelIndex::CH6;
  }
  if(idx == "7") {
    return ChannelIndex::CH7;
  }
  if(idx == "8") {
    return ChannelIndex::CH8;
  }
  if(idx == "9") {
    return ChannelIndex::CH9;
  }
  if(idx == "A") {
    return ChannelIndex::A;
  }
  if(idx == "B") {
    return ChannelIndex::B;
  }
  if(idx == "C") {
    return ChannelIndex::C;
  }
  if(idx == "D") {
    return ChannelIndex::D;
  }
  if(idx == "E") {
    return ChannelIndex::E;
  }
  if(idx == "F") {
    return ChannelIndex::F;
  }
  return ChannelIndex::NONE;
}

inline std::string to_string(joda::settings::ChannelIndex idx)
{
  switch(idx) {
    case ChannelIndex::NONE:
      return "None";
    case ChannelIndex::CH0:
      return "0";
    case ChannelIndex::CH1:
      return "1";
    case ChannelIndex::CH2:
      return "2";
    case ChannelIndex::CH3:
      return "3";
    case ChannelIndex::CH4:
      return "4";
    case ChannelIndex::CH5:
      return "5";
    case ChannelIndex::CH6:
      return "6";
    case ChannelIndex::CH7:
      return "7";
    case ChannelIndex::CH8:
      return "8";
    case ChannelIndex::CH9:
      return "9";
    case ChannelIndex::A:
      return "A";
    case ChannelIndex::B:
      return "B";
    case ChannelIndex::C:
      return "C";
    case ChannelIndex::D:
      return "D";
    case ChannelIndex::E:
      return "E";
    case ChannelIndex::F:
      return "F";
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
