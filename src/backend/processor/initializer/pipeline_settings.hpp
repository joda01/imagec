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
struct PipelineSettings
{
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
  enums::ZProjection zProjection = enums::ZProjection::UNDEFINED;

  //
  // Default cluster ID of this pipeline. Can be accessed with $
  //
  enums::ClusterIdIn defaultClusterId = enums::ClusterIdIn::NONE;

  void check() const
  {
    CHECK(zProjection != enums::ZProjection::UNDEFINED, "Define the z-projection mode for image loading in pipeline!");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(PipelineSettings, source, cStackIndex, tStackIndex, zStackIndex,
                                                       zProjection, defaultClusterId);
};

NLOHMANN_JSON_SERIALIZE_ENUM(PipelineSettings::Source, {
                                                           {PipelineSettings::Source::FROM_FILE, "FromFile"},
                                                           {PipelineSettings::Source::FROM_MEMORY, "FromMemory"},
                                                           {PipelineSettings::Source::BLANK, "FromBlank"},

                                                       });

}    // namespace joda::settings
