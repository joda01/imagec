#pragma once

#include <qsettings.h>
#include <cstdint>
#include <set>
#include "backend/enums/enums_clusters.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ProjectImageSetup
{
  enum class ZStackHandling
  {
    EXACT_ONE,
    INTENSITY_PROJECTION,
    EACH_ONE
  };

  enum class TStackHandling
  {
    EXACT_ONE,
    EACH_ONE
  };

  enum class CStackHandling
  {
    EXACT_ONE,
    EACH_ONE
  };

  ZStackHandling zStackHandling = ZStackHandling::INTENSITY_PROJECTION;
  TStackHandling tStackHandling = TStackHandling::EACH_ONE;
  CStackHandling cStackHandling = CStackHandling::EXACT_ONE;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ProjectImageSetup, zStackHandling, tStackHandling,
                                                       cStackHandling);

  void check() const
  {
  }
};

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::ZStackHandling,
                             {{ProjectImageSetup::ZStackHandling::EXACT_ONE, "ExactOne"},
                              {ProjectImageSetup::ZStackHandling::EACH_ONE, "EachOne"},
                              {ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION, "IntensityProjection"}});

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::TStackHandling,
                             {
                                 {ProjectImageSetup::TStackHandling::EXACT_ONE, "ExactOne"},
                                 {ProjectImageSetup::TStackHandling::EACH_ONE, "EachOne"},
                             });

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::CStackHandling,
                             {
                                 {ProjectImageSetup::CStackHandling::EXACT_ONE, "ExactOne"},
                                 {ProjectImageSetup::CStackHandling::EACH_ONE, "EachOne"},
                             });

}    // namespace joda::settings
