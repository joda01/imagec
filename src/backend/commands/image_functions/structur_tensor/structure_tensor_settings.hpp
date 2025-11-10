#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct StructureTensorSettings : public SettingBase
{
public:
  enum class Mode
  {
    StructureTensorCoherence,
    StructureTensorEigenvaluesX,
    StructureTensorEigenvaluesY,
  };

  //
  //
  //
  Mode mode = Mode::StructureTensorCoherence;

  //
  //
  //
  int32_t kernelSize = 5;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(kernelSize % 2 == 1, "Kernel size must be an odd number.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(StructureTensorSettings, mode, kernelSize);
};

NLOHMANN_JSON_SERIALIZE_ENUM(StructureTensorSettings::Mode,
                             {
                                 {StructureTensorSettings::Mode::StructureTensorCoherence, "StructureTensorCoherence"},
                                 {StructureTensorSettings::Mode::StructureTensorEigenvaluesX, "StructureTensorEigenvaluesX"},
                                 {StructureTensorSettings::Mode::StructureTensorEigenvaluesX, "StructureTensorEigenvaluesY"},
                             });

}    // namespace joda::settings
