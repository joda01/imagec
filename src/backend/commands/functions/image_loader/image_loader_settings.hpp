#pragma once

#include <cstdint>
#include <set>
#include "backend/commands/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::cmd::functions {

struct ImageLoaderSettings : public cmd::Setting
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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImageLoaderSettings, imageInputDirectory, zStackHandling, tStackHandling);

  void check() const override
  {
  }
};

NLOHMANN_JSON_SERIALIZE_ENUM(ImageLoaderSettings::ZStackHandling,
                             {{ImageLoaderSettings::ZStackHandling::EXACT_ONE, "ExactOne"},
                              {ImageLoaderSettings::ZStackHandling::EACH_INDIVIDUAL, "EachIndividual"},
                              {ImageLoaderSettings::ZStackHandling::INTENSITY_PROJECTION, "IntensityProjection"}});

NLOHMANN_JSON_SERIALIZE_ENUM(ImageLoaderSettings::TStackHandling,
                             {
                                 {ImageLoaderSettings::TStackHandling::EXACT_ONE, "ExactOne"},
                                 {ImageLoaderSettings::TStackHandling::EACH_INDIVIDUAL, "EachIndividual"},
                             });

}    // namespace joda::cmd::functions
