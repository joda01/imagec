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
    EACH_INDIVIDUAL
  };

  enum class TStackHandling
  {
    EXACT_ONE,
    EACH_INDIVIDUAL
  };

  ZStackHandling zStackHandling;
  TStackHandling tStackHandling;

  std::string imageInputDirectory;
  std::string resultsOutputFolder;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineInitializerSettings, imageInputDirectory, resultsOutputFolder,
                                              zStackHandling, tStackHandling);

  void check() const override
  {
  }
};

NLOHMANN_JSON_SERIALIZE_ENUM(PipelineInitializerSettings::ZStackHandling,
                             {{PipelineInitializerSettings::ZStackHandling::EXACT_ONE, "ExactOne"},
                              {PipelineInitializerSettings::ZStackHandling::EACH_INDIVIDUAL, "EachIndividual"},
                              {PipelineInitializerSettings::ZStackHandling::INTENSITY_PROJECTION,
                               "IntensityProjection"}});

NLOHMANN_JSON_SERIALIZE_ENUM(PipelineInitializerSettings::TStackHandling,
                             {
                                 {PipelineInitializerSettings::TStackHandling::EXACT_ONE, "ExactOne"},
                                 {PipelineInitializerSettings::TStackHandling::EACH_INDIVIDUAL, "EachIndividual"},
                             });

}    // namespace joda::settings
