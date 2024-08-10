
#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>

namespace joda::settings {

class ThresholdSettings final
{
public:
  /////////////////////////////////////////////////////
  enum class Mode
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

  //
  // Which threshold algorithm should be used
  // [MANUAL, LI, MIN_ERROR, TRIANGLE]
  //
  Mode mode = Mode::NONE;

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

  struct Watershed
  {
    bool enabled = false;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Watershed, enabled);
  };

  //
  // Enable watershed segmentation
  //
  Watershed $watershedSegmentation;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ThresholdSettings, mode, thresholdMin, thresholdMax,
                                              $watershedSegmentation);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ThresholdSettings::Mode, {{ThresholdSettings::Mode::NONE, ""},
                                                       {ThresholdSettings::Mode::MANUAL, "Manual"},
                                                       {ThresholdSettings::Mode::LI, "Li"},
                                                       {ThresholdSettings::Mode::MIN_ERROR, "MinError"},
                                                       {ThresholdSettings::Mode::TRIANGLE, "Triangle"},
                                                       {ThresholdSettings::Mode::MOMENTS, "Moments"},
                                                       {ThresholdSettings::Mode::HUANG, "Huang"},
                                                       {ThresholdSettings::Mode::INTERMODES, "Intermodes"},
                                                       {ThresholdSettings::Mode::ISODATA, "IsoData"},
                                                       {ThresholdSettings::Mode::MAX_ENTROPY, "MaxEntropy"},
                                                       {ThresholdSettings::Mode::MEAN, "Mean"},
                                                       {ThresholdSettings::Mode::MINIMUM, "Minimum"},
                                                       {ThresholdSettings::Mode::OTSU, "Otsu"},
                                                       {ThresholdSettings::Mode::PERCENTILE, "Percentil"},
                                                       {ThresholdSettings::Mode::RENYI_ENTROPY, "TenyiEntropy"},
                                                       {ThresholdSettings::Mode::SHANBHAG, "Shanbhag"},
                                                       {ThresholdSettings::Mode::YEN, "Yen"}})

}    // namespace joda::settings
