#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class ZStackProcessing
{
public:
  enum ZStackMethod
  {
    NONE,
    MAX_INTENSITY,
    PROJECT_3D
  };

  ZStackMethod method = ZStackMethod::NONE;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ZStackProcessing, method);
};
NLOHMANN_JSON_SERIALIZE_ENUM(ZStackProcessing::ZStackMethod,
                             {{ZStackProcessing::ZStackMethod::NONE, "Off"},
                              {ZStackProcessing::ZStackMethod::MAX_INTENSITY, "MaxIntensity"},
                              {ZStackProcessing::ZStackMethod::PROJECT_3D, "Project3D"}});

}    // namespace joda::settings
