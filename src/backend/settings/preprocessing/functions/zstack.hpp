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

  NLOHMANN_JSON_SERIALIZE_ENUM(ZStackMethod, {{ZStackMethod::NONE, ""},
                                              {ZStackMethod::MAX_INTENSITY, "MaxIntensity"},
                                              {ZStackMethod::PROJECT_3D, "Project3D"}});

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ZStackProcessing, method);
};
}    // namespace joda::settings
