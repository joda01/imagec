#pragma once

#include <qsettings.h>
#include <cstdint>
#include <set>

#include "backend/enums/enums_units.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ProjectPipelineSetup
{
  //
  // The unit which is used in the pipeline commands for real size values (e.g. area size, distance, ...)
  // This value is used for pipeline execution. In comparison to the imagePixelSizeSettings.pixelSizeUnit this
  // value is not written to the database it is just used for the commands.
  //
  enums::Units realSizesUnit = enums::Units::um;

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ProjectPipelineSetup, realSizesUnit);
};
}    // namespace joda::settings
