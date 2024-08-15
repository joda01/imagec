#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ImageSaverSettings
{
public:
  //
  // PNG compression level (0 = no compression)
  //
  int32_t compression = 0;

  //
  // Image name prefix
  //
  std::string namePrefix;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageSaverSettings, compression, namePrefix);
};
}    // namespace joda::settings
