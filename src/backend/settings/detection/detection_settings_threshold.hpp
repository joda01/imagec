
#pragma once

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
  uint16_t thresholdMax = 0;

  //
  // Enable watershed segmentation
  //
  bool $watershedSegmentation = false;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ThresholdSettings, mode, thresholdMin, thresholdMax,
                                              $watershedSegmentation);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ThresholdSettings::Mode, {{ThresholdSettings::Mode::NONE, ""},
                                                       {ThresholdSettings::Mode::MANUAL, "Manual"},
                                                       {ThresholdSettings::Mode::LI, "li"},
                                                       {ThresholdSettings::Mode::MIN_ERROR, "minError"},
                                                       {ThresholdSettings::Mode::TRIANGLE, "triangle"},
                                                       {ThresholdSettings::Mode::MOMENTS, "moments"},
                                                       {ThresholdSettings::Mode::HUANG, "huang"},
                                                       {ThresholdSettings::Mode::INTERMODES, "intermodes"},
                                                       {ThresholdSettings::Mode::ISODATA, "isodata"},
                                                       {ThresholdSettings::Mode::MAX_ENTROPY, "maxEntropy"},
                                                       {ThresholdSettings::Mode::MEAN, "mean"},
                                                       {ThresholdSettings::Mode::MINIMUM, "minimum"},
                                                       {ThresholdSettings::Mode::OTSU, "otsu"},
                                                       {ThresholdSettings::Mode::PERCENTILE, "percentil"},
                                                       {ThresholdSettings::Mode::RENYI_ENTROPY, "renyiEntropy"},
                                                       {ThresholdSettings::Mode::SHANBHAG, "shanbhag"},
                                                       {ThresholdSettings::Mode::YEN, "yen"}})

}    // namespace joda::settings
