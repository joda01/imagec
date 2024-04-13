
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

  NLOHMANN_JSON_SERIALIZE_ENUM(Mode, {{Mode::NONE, ""},
                                      {Mode::MANUAL, "Threshold"},
                                      {Mode::LI, "Ai"},
                                      {Mode::MIN_ERROR, "Ai"},
                                      {Mode::TRIANGLE, "Ai"},
                                      {Mode::MOMENTS, "Ai"},
                                      {Mode::HUANG, "Ai"},
                                      {Mode::INTERMODES, "Ai"},
                                      {Mode::ISODATA, "Ai"},
                                      {Mode::MAX_ENTROPY, "Ai"},
                                      {Mode::MEAN, "Ai"},
                                      {Mode::MINIMUM, "Ai"},
                                      {Mode::OTSU, "Ai"},
                                      {Mode::PERCENTILE, "Ai"},
                                      {Mode::RENYI_ENTROPY, "Ai"},
                                      {Mode::SHANBHAG, "Ai"},
                                      {Mode::YEN, "Ai"}})

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ThresholdSettings, mode, thresholdMin, thresholdMax,
                                              $watershedSegmentation);
};

}    // namespace joda::settings
