#pragma once

#include <cstdint>
#include <list>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ThresholdSettings : public SettingBase
{
public:
  enum class Methods
  {
    NONE,
    MANUAL,
    LI,
    MIN_ERROR,
    TRIANGLE,
    MOMENTS,
    HUANG,
    INTERMODES,
    ISODATA,
    MAX_ENTROPY,
    MEAN,
    MINIMUM,
    OTSU,
    PERCENTILE,
    RENYI_ENTROPY,
    SHANBHAG,
    YEN
  };

  struct Threshold
  {
    //
    // Which threshold algorithm should be used
    // [MANUAL, LI, MIN_ERROR, TRIANGLE]
    //
    Methods method = Methods::MANUAL;

    //
    // Minimum threshold value.
    // [0-65535]
    //
    uint16_t thresholdMin = 0;

    //
    // Maximum threshold value (default 65535)
    // [0-65535]
    //
    uint16_t thresholdMax = UINT16_MAX;

    //
    // Grayscale to assign to
    //
    int32_t modelClassId = UINT16_MAX;

    /////////////////////////////////////////////////////
    void check() const
    {
      CHECK_ERROR(thresholdMax >= thresholdMin, "Threshold max must be higher than threshold min.");
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Threshold, method, thresholdMin, thresholdMax, modelClassId);
  };

  std::list<Threshold> modelClasses;

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ThresholdSettings, modelClasses);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ThresholdSettings::Methods, {{ThresholdSettings::Methods::NONE, ""},
                                                          {ThresholdSettings::Methods::MANUAL, "Manual"},
                                                          {ThresholdSettings::Methods::LI, "Li"},
                                                          {ThresholdSettings::Methods::MIN_ERROR, "MinError"},
                                                          {ThresholdSettings::Methods::TRIANGLE, "Triangle"},
                                                          {ThresholdSettings::Methods::MOMENTS, "Moments"},
                                                          {ThresholdSettings::Methods::HUANG, "Huang"},
                                                          {ThresholdSettings::Methods::INTERMODES, "Intermodes"},
                                                          {ThresholdSettings::Methods::ISODATA, "IsoData"},
                                                          {ThresholdSettings::Methods::MAX_ENTROPY, "MaxEntropy"},
                                                          {ThresholdSettings::Methods::MEAN, "Mean"},
                                                          {ThresholdSettings::Methods::MINIMUM, "Minimum"},
                                                          {ThresholdSettings::Methods::OTSU, "Otsu"},
                                                          {ThresholdSettings::Methods::PERCENTILE, "Percentil"},
                                                          {ThresholdSettings::Methods::RENYI_ENTROPY, "TenyiEntropy"},
                                                          {ThresholdSettings::Methods::SHANBHAG, "Shanbhag"},
                                                          {ThresholdSettings::Methods::YEN, "Yen"}})

}    // namespace joda::settings
