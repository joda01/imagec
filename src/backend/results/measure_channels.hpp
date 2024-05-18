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

#include <cstdint>
#include <stdexcept>
#include "backend/image_processing/roi/roi.hpp"
#include "backend/settings/channel/channel_index.hpp"

namespace joda::results {

enum class MeasureChannel : uint16_t
{
  CONFIDENCE                  = 1,
  AREA_SIZE                   = 2,
  PERIMETER                   = 3,
  CIRCULARITY                 = 4,
  CENTER_OF_MASS_X            = 5,
  CENTER_OF_MASS_Y            = 6,
  BOUNDING_BOX_WIDTH          = 7,
  BOUNDING_BOX_HEIGHT         = 8,
  INTENSITY_AVG               = 9,
  INTENSITY_MIN               = 10,
  INTENSITY_MAX               = 11,
  CROSS_CHANNEL_INTENSITY_AVG = 12,
  CROSS_CHANNEL_INTENSITY_MIN = 13,
  CROSS_CHANNEL_INTENSITY_MAX = 14,
  CROSS_CHANNEL_COUNT         = 15
};

enum class ChannelIndex : uint16_t
{
  ME  = 0,
  CH0 = 1,
  CH1 = 2,
  CH2 = 3,
  CH3 = 4,
  CH4 = 5,
  CH5 = 6,
  CH6 = 7,
  CH7 = 8,
  CH8 = 9,
  CH9 = 10,
  A   = 65,
  B   = 66,
  C   = 67,
  D   = 68,
  E   = 69,
  F   = 70
};

enum class ObjectValidity : uint32_t
{

  VALID                = 0x00,
  TOO_SMALL            = 0x02,
  TOO_BIG              = 0x04,
  TOO_LESS_CIRCULARITY = 0x08,
  TOO_LESS_OVERLAPPING = 0x10,
  REFERENCE_SPOT       = 0x20,
  INVALID              = 0x40,
  AT_THE_EDGE          = 0x80,
  UNKNOWN              = 0xFFFFFFFF,

};

class MeasureChannelKey
{
public:
  static constexpr uint32_t MEASURE_CH_SHIFT = 16;

  MeasureChannelKey(MeasureChannel measureCh, ChannelIndex idx) :
      mChannelIndex((static_cast<uint32_t>(measureCh) << MEASURE_CH_SHIFT) | static_cast<uint32_t>(idx))
  {
  }

  [[nodiscard]] uint32_t getKey() const
  {
    return mChannelIndex;
  }

  explicit operator uint32_t() const
  {
    return mChannelIndex;
  }

private:
  uint32_t mChannelIndex;
};

inline ChannelIndex toMeasureChannelIndex(joda::settings::ChannelIndex idx)
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

inline uint32_t toValidity(image::ParticleValidity validity)
{
  switch(validity) {
    case image::ParticleValidity::UNKNOWN:
      return static_cast<uint32_t>(ObjectValidity::UNKNOWN);
    case image::ParticleValidity::VALID:
      return static_cast<uint32_t>(ObjectValidity::VALID);
    default:
      return static_cast<uint32_t>(validity);
  }
}

}    // namespace joda::results
