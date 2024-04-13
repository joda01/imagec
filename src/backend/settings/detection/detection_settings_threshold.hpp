
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
                                                       {ThresholdSettings::Mode::MANUAL, "Threshold"},
                                                       {ThresholdSettings::Mode::LI, "Ai"},
                                                       {ThresholdSettings::Mode::MIN_ERROR, "Ai"},
                                                       {ThresholdSettings::Mode::TRIANGLE, "Ai"},
                                                       {ThresholdSettings::Mode::MOMENTS, "Ai"},
                                                       {ThresholdSettings::Mode::HUANG, "Ai"},
                                                       {ThresholdSettings::Mode::INTERMODES, "Ai"},
                                                       {ThresholdSettings::Mode::ISODATA, "Ai"},
                                                       {ThresholdSettings::Mode::MAX_ENTROPY, "Ai"},
                                                       {ThresholdSettings::Mode::MEAN, "Ai"},
                                                       {ThresholdSettings::Mode::MINIMUM, "Ai"},
                                                       {ThresholdSettings::Mode::OTSU, "Ai"},
                                                       {ThresholdSettings::Mode::PERCENTILE, "Ai"},
                                                       {ThresholdSettings::Mode::RENYI_ENTROPY, "Ai"},
                                                       {ThresholdSettings::Mode::SHANBHAG, "Ai"},
                                                       {ThresholdSettings::Mode::YEN, "Ai"}})

}    // namespace joda::settings
