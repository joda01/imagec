#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include <stdexcept>
#include "backend/enums/enum_objects.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/json_optional_parser_helper.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

///
/// \class
/// \author
/// \brief
///
struct PipelineSettings : public Setting
{
  enum class ZProjection
  {
    NONE,
    MAX_INTENSITY,
    MIN_INTENSITY,
    AVG_INTENSITY
  };

  enum class Source
  {
    FROM_FILE,
    FROM_MEMORY,
    BLANK
  };

  //
  // From where the inital image for this pipeline shoule be loaded
  //
  Source source = Source::FROM_FILE;

  //
  // Image channel to load
  //
  enums::cStack_t cStackIndex = 0;

  //
  // Image Time-Stack to load (Is only used if tStackHandling is set to EXACT_ONE)
  //
  enums::tStack_t tStackIndex = 0;

  //
  // Image Z-Stack to load (Is only used if zStackIndex is set to EXACT_ONE)
  //
  enums::zStack_t zStackIndex = 0;

  //
  // Is only used if zStackHandling is set to INTENSITY_PROJECTION
  //
  ZProjection zProjection = ZProjection::NONE;

  //
  // Default cluster ID of this pipeline. Can be accessed with $
  //
  enums::ClusterId defaultClusterId = enums::ClusterId::UNDEFINED;

  //
  // Default object store ID of this pipeline.
  //
  enums::MemoryIdx defaultObjectStoreId;

  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineSettings, source, cStackIndex, tStackIndex, zStackIndex,
                                              zProjection, defaultClusterId, defaultObjectStoreId);
};

NLOHMANN_JSON_SERIALIZE_ENUM(PipelineSettings::Source, {
                                                           {PipelineSettings::Source::FROM_FILE, "FromFile"},
                                                           {PipelineSettings::Source::FROM_MEMORY, "FromMemory"},
                                                           {PipelineSettings::Source::BLANK, "FromBlank"},

                                                       });

NLOHMANN_JSON_SERIALIZE_ENUM(PipelineSettings::ZProjection,
                             {
                                 {PipelineSettings::ZProjection::NONE, "None"},
                                 {PipelineSettings::ZProjection::MAX_INTENSITY, "MaxIntensity"},
                                 {PipelineSettings::ZProjection::MIN_INTENSITY, "MinIntensity"},
                                 {PipelineSettings::ZProjection::AVG_INTENSITY, "AvgIntensity"},
                             });

}    // namespace joda::settings
