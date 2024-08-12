#pragma once

#include <qsettings.h>
#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct PipelineInitializerSettings : public Setting
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

  std::string imageInputDirectory;
  std::string resultsOutputFolder;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineInitializerSettings, imageInputDirectory, resultsOutputFolder,
                                              zStackHandling, tStackHandling, cStackHandling);

  void check() const override
  {
  }
};

NLOHMANN_JSON_SERIALIZE_ENUM(PipelineInitializerSettings::ZStackHandling,
                             {{PipelineInitializerSettings::ZStackHandling::EXACT_ONE, "ExactOne"},
                              {PipelineInitializerSettings::ZStackHandling::EACH_ONE, "EachOne"},
                              {PipelineInitializerSettings::ZStackHandling::INTENSITY_PROJECTION,
                               "IntensityProjection"}});

NLOHMANN_JSON_SERIALIZE_ENUM(PipelineInitializerSettings::TStackHandling,
                             {
                                 {PipelineInitializerSettings::TStackHandling::EXACT_ONE, "ExactOne"},
                                 {PipelineInitializerSettings::TStackHandling::EACH_ONE, "EachOne"},
                             });

NLOHMANN_JSON_SERIALIZE_ENUM(PipelineInitializerSettings::CStackHandling,
                             {
                                 {PipelineInitializerSettings::CStackHandling::EXACT_ONE, "ExactOne"},
                                 {PipelineInitializerSettings::CStackHandling::EACH_ONE, "EachOne"},
                             });

}    // namespace joda::settings
