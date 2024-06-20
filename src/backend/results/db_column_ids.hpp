///
/// \file      measure_channels.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/roi/roi.hpp"
#include "backend/settings/channel/channel_index.hpp"

namespace joda::results {

enum class Stats
{
  AVG    = 0,
  MEDIAN = 1,
  SUM    = 2,
  MIN    = 3,
  MAX    = 4,
  STDDEV = 5,
  CNT    = 6
};

inline std::string toString(Stats stats)
{
  switch(stats) {
    case Stats::AVG:
      return "AVG";
    case Stats::MEDIAN:
      return "MEDIAN";
    case Stats::SUM:
      return "SUM";
    case Stats::MIN:
      return "MIN";
    case Stats::MAX:
      return "MAX";
    case Stats::STDDEV:
      return "STDDEV";
    case Stats::CNT:
      return "CNT";
  }
  return "";
}

enum class MeasureChannel : uint16_t
{
  CONFIDENCE                  = 1,
  AREA_SIZE                   = 2,
  PERIMETER                   = 3,
  CIRCULARITY                 = 4,
  VALID                       = 5,
  INVALID                     = 6,
  CENTER_OF_MASS_X            = 7,
  CENTER_OF_MASS_Y            = 8,
  BOUNDING_BOX_WIDTH          = 9,
  BOUNDING_BOX_HEIGHT         = 10,
  INTENSITY_AVG               = 11,
  INTENSITY_MIN               = 12,
  INTENSITY_MAX               = 13,
  CROSS_CHANNEL_INTENSITY_AVG = 14,
  CROSS_CHANNEL_INTENSITY_MIN = 15,
  CROSS_CHANNEL_INTENSITY_MAX = 16,
  CROSS_CHANNEL_COUNT         = 17,

};

inline std::string toString(MeasureChannel ch)
{
  switch(ch) {
    case MeasureChannel::CONFIDENCE:
      return "CONFIDENCE";
    case MeasureChannel::AREA_SIZE:
      return "AREA_SIZE";
    case MeasureChannel::PERIMETER:
      return "PERIMETER";
    case MeasureChannel::CIRCULARITY:
      return "CIRCULARITY";
    case MeasureChannel::VALID:
      return "VALID";
    case MeasureChannel::INVALID:
      return "INVALID";
    case MeasureChannel::CENTER_OF_MASS_X:
      return "CENTER_OF_MASS_X";
    case MeasureChannel::CENTER_OF_MASS_Y:
      return "CENTER_OF_MASS_Y";
    case MeasureChannel::BOUNDING_BOX_WIDTH:
      return "BOUNDING_BOX_WIDTH";
    case MeasureChannel::BOUNDING_BOX_HEIGHT:
      return "BOUNDING_BOX_HEIGHT";
    case MeasureChannel::INTENSITY_AVG:
      return "INTENSITY_AVG";
    case MeasureChannel::INTENSITY_MIN:
      return "INTENSITY_MIN";
    case MeasureChannel::INTENSITY_MAX:
      return "INTENSITY_MAX";
    case MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG:
      return "CROSS_CHANNEL_INTENSITY_AVG";
    case MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN:
      return "CROSS_CHANNEL_INTENSITY_MIN";
    case MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX:
      return "CROSS_CHANNEL_INTENSITY_MAX";
    case MeasureChannel::CROSS_CHANNEL_COUNT:
      return "CROSS_CHANNEL_COUNT";
  }
  return "";
}

enum class ChannelIndex : uint16_t
{
  CH0 = 0,
  CH1 = 1,
  CH2 = 2,
  CH3 = 3,
  CH4 = 4,
  CH5 = 5,
  CH6 = 6,
  CH7 = 7,
  CH8 = 8,
  CH9 = 9,
  A   = 65,
  B   = 66,
  C   = 67,
  D   = 68,
  E   = 69,
  F   = 70,
  ME  = 0xFFFF
};

inline std::string toString(ChannelIndex ch)
{
  switch(ch) {
    case ChannelIndex::CH0:
      return "CH0";
    case ChannelIndex::CH1:
      return "CH1";
    case ChannelIndex::CH2:
      return "CH2";
    case ChannelIndex::CH3:
      return "CH3";
    case ChannelIndex::CH4:
      return "CH4";
    case ChannelIndex::CH5:
      return "CH5";
    case ChannelIndex::CH6:
      return "CH6";
    case ChannelIndex::CH7:
      return "CH7";
    case ChannelIndex::CH8:
      return "CH8";
    case ChannelIndex::CH9:
      return "CH9";
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
    default:
    case ChannelIndex::ME:
      return "ME";
  }
}

enum class ObjectValidityEnum : size_t
{
  UNKNOWN              = 1,
  INVALID              = 2,
  MANUAL_OUT_SORTED    = 3,
  TOO_SMALL            = 4,
  TOO_BIG              = 5,
  TOO_LESS_CIRCULARITY = 6,
  TOO_LESS_OVERLAPPING = 7,
  REFERENCE_SPOT       = 8,
  AT_THE_EDGE          = 9,
};

struct ObjectValidity : public std::bitset<32>
{
  using bitset<32>::bitset;

  bool test(ObjectValidityEnum idx)
  {
    return bitset<32>::test(static_cast<size_t>(idx));
  }
};

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
};

class MeasureChannelId
{
public:
  static constexpr uint32_t MEASURE_CH_SHIFT = 16;
  MeasureChannelId()
  {
  }

  MeasureChannelId(uint32_t val) : mChannelIndex(val)
  {
  }

  MeasureChannelId(MeasureChannel measureCh, ChannelIndex idx) :
      mChannelIndex((static_cast<uint32_t>(measureCh) << MEASURE_CH_SHIFT) | static_cast<uint32_t>(idx))
  {
  }

  [[nodiscard]] uint32_t getKey() const
  {
    return mChannelIndex;
  }

  [[nodiscard]] ChannelIndex getChannelIndex() const
  {
    return static_cast<ChannelIndex>(mChannelIndex & 0xFFFF);
  }

  [[nodiscard]] MeasureChannel getMeasureChannel() const
  {
    return static_cast<MeasureChannel>(mChannelIndex >> MEASURE_CH_SHIFT);
  }

  explicit operator uint32_t() const
  {
    return mChannelIndex;
  }

  bool operator<(const MeasureChannelId &in) const
  {
    return mChannelIndex < static_cast<uint32_t>(in);
  }

  [[nodiscard]] std::string toString() const
  {
    std::string txt;
    switch(getMeasureChannel()) {
      case MeasureChannel::CONFIDENCE:
        txt = "Confidence";
        break;
      case MeasureChannel::AREA_SIZE:
        txt = "Area size";
        break;
      case MeasureChannel::PERIMETER:
        txt = "Perimeter";
        break;
      case MeasureChannel::CIRCULARITY:
        txt = "Circularity";
        break;
      case MeasureChannel::VALID:
        txt = "Valid";
        break;
      case MeasureChannel::INVALID:
        txt = "Invalid";
        break;
      case MeasureChannel::CENTER_OF_MASS_X:
        txt = "X";
        break;
      case MeasureChannel::CENTER_OF_MASS_Y:
        txt = "Y";
        break;
      case MeasureChannel::BOUNDING_BOX_WIDTH:
        txt = "Bounding box width";
        break;
      case MeasureChannel::BOUNDING_BOX_HEIGHT:
        txt = "Bounding box height";
        break;
      case MeasureChannel::INTENSITY_AVG:
        txt = "Intensity AVG";
        break;
      case MeasureChannel::INTENSITY_MIN:
        txt = "Intensity Min";
        break;
      case MeasureChannel::INTENSITY_MAX:
        txt = "Intensity Max";
        break;
      case MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG:
        txt = "Intensity AVG";
        break;
      case MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN:
        txt = "Intensity Min";
        break;
      case MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX:
        txt = "Intensity Max";
        break;
      case MeasureChannel::CROSS_CHANNEL_COUNT:
        txt = "Cross count";
        break;
    }

    switch(getChannelIndex()) {
      case ChannelIndex::ME:
        break;
      case ChannelIndex::CH0:
        txt += "(0)";
        break;
      case ChannelIndex::CH1:
        txt += "(1)";
        break;
      case ChannelIndex::CH2:
        txt += "(2)";
        break;
      case ChannelIndex::CH3:
        txt += "(3)";
        break;
      case ChannelIndex::CH4:
        txt += "(4)";
        break;
      case ChannelIndex::CH5:
        txt += "(5)";
        break;
      case ChannelIndex::CH6:
        txt += "(6)";
        break;
      case ChannelIndex::CH7:
        txt += "(7)";
        break;
      case ChannelIndex::CH8:
        txt += "(8)";
        break;
      case ChannelIndex::CH9:
        txt += "(9)";
        break;
      case ChannelIndex::A:
        txt += "(A)";
        break;
      case ChannelIndex::B:
        txt += "(B)";
        break;
      case ChannelIndex::C:
        txt += "(C)";
        break;
      case ChannelIndex::D:
        txt += "(D)";
        break;
      case ChannelIndex::E:
        txt += "(E)";
        break;
      case ChannelIndex::F:
        txt += "(F)";
        break;
    }
    return txt;
  }

private:
  uint32_t mChannelIndex = 0;
};

inline ChannelIndex toChannelIndex(joda::settings::ChannelIndex idx)
{
  switch(idx) {
    case settings::ChannelIndex::NONE:
      return ChannelIndex::ME;
    case settings::ChannelIndex::CH0:
      return ChannelIndex::CH0;
    case settings::ChannelIndex::CH1:
      return ChannelIndex::CH1;
    case settings::ChannelIndex::CH2:
      return ChannelIndex::CH2;
    case settings::ChannelIndex::CH3:
      return ChannelIndex::CH3;
    case settings::ChannelIndex::CH4:
      return ChannelIndex::CH4;
    case settings::ChannelIndex::CH5:
      return ChannelIndex::CH5;
    case settings::ChannelIndex::CH6:
      return ChannelIndex::CH6;
    case settings::ChannelIndex::CH7:
      return ChannelIndex::CH7;
    case settings::ChannelIndex::CH8:
      return ChannelIndex::CH8;
    case settings::ChannelIndex::CH9:
      return ChannelIndex::CH9;
    case settings::ChannelIndex::A:
      return ChannelIndex::A;
    case settings::ChannelIndex::B:
      return ChannelIndex::B;
    case settings::ChannelIndex::C:
      return ChannelIndex::C;
    case settings::ChannelIndex::D:
      return ChannelIndex::D;
    case settings::ChannelIndex::E:
      return ChannelIndex::E;
    case settings::ChannelIndex::F:
      return ChannelIndex::F;
    default:
      throw std::runtime_error("Unknown channel");
  }
}

inline ObjectValidity toValidity(image::ParticleValidity validity)
{
  ObjectValidity ret{};
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::UNKNOWN))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::UNKNOWN));
  }
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::INVALID))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::INVALID));
  }
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::TOO_SMALL))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::TOO_SMALL));
  }
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::TOO_BIG))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::TOO_BIG));
  }
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::TOO_LESS_CIRCULARITY))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::TOO_LESS_CIRCULARITY));
  }
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::TOO_LESS_OVERLAPPING))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::TOO_LESS_OVERLAPPING));
  }
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::REFERENCE_SPOT))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::REFERENCE_SPOT));
  }
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::AT_THE_EDGE))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::AT_THE_EDGE));
  }
  if(validity.test(static_cast<size_t>(ObjectValidityEnum::MANUAL_OUT_SORTED))) {
    ret.set(static_cast<size_t>(ObjectValidityEnum::MANUAL_OUT_SORTED));
  }

  return ret;
}

inline ChannelValidity toChannelValidity(joda::image::detect::ResponseDataValidity validity)
{
  ChannelValidity ret{};

  if(validity.test(static_cast<size_t>(ChannelValidityEnum::UNKNOWN))) {
    ret.set(static_cast<size_t>(ChannelValidityEnum::UNKNOWN));
  }
  if(validity.test(static_cast<size_t>(ChannelValidityEnum::INVALID))) {
    ret.set(static_cast<size_t>(ChannelValidityEnum::INVALID));
  }
  if(validity.test(static_cast<size_t>(ChannelValidityEnum::MANUAL_OUT_SORTED))) {
    ret.set(static_cast<size_t>(ChannelValidityEnum::MANUAL_OUT_SORTED));
  }
  if(validity.test(static_cast<size_t>(ChannelValidityEnum::POSSIBLE_NOISE))) {
    ret.set(static_cast<size_t>(ChannelValidityEnum::POSSIBLE_NOISE));
  }
  if(validity.test(static_cast<size_t>(ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD))) {
    ret.set(static_cast<size_t>(ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD));
  }
  return ret;
}

}    // namespace joda::results
