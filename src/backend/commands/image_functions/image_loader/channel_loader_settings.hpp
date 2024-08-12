#pragma once

#include <cstdint>
#include <set>
#include "backend/commands/setting.hpp"
#include "backend/global_enums.hpp"
#include <nlohmann/json.hpp>

namespace joda::cmd::functions {

struct ChannelLoaderSettings : public cmd::Setting
{
  enum class ZProjection
  {
    NONE,
    MAX_INTENSITY,
    MIN_INTENSITY,
    AVG_INTENSITY
  };

  //
  // The input slot used to to load.
  //
  joda::enums::Slot input = joda::enums::Slot::$;

  //
  // Image channel to load
  //
  joda::enums::ImageChannelIndex imageChannelIndex = joda::enums::ImageChannelIndex::NONE;

  //
  // Is only used if zStackHandling is set to INTENSITY_PROJECTION
  //
  ZProjection zProjection = ZProjection::NONE;

  //
  // Is only used if zStackIndex is set to EXACT_ONE
  //
  int32_t zStackIndex = 0;

  //
  // Is only used if tStackHandling is set to EXACT_ONE
  //
  int32_t tStackIndex = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelLoaderSettings, input, imageChannelIndex, zProjection, zStackIndex,
                                              tStackIndex);

  void check() const override
  {
  }
};

NLOHMANN_JSON_SERIALIZE_ENUM(ChannelLoaderSettings::ZProjection,
                             {
                                 {ChannelLoaderSettings::ZProjection::NONE, "None"},
                                 {ChannelLoaderSettings::ZProjection::MAX_INTENSITY, "MaxIntensity"},
                                 {ChannelLoaderSettings::ZProjection::MIN_INTENSITY, "MinIntensity"},
                                 {ChannelLoaderSettings::ZProjection::AVG_INTENSITY, "AvgIntensity"},
                             });

}    // namespace joda::cmd::functions
