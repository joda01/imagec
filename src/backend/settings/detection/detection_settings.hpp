
#pragma once

#include <nlohmann/json.hpp>
#include "detection_settings_ai.hpp"
#include "detection_settings_threshold.hpp"

namespace joda::settings {

class DetectionSettings
{
  enum class DetectionMode
  {
    NONE,
    THRESHOLD,
    AI
  };

  DetectionMode detectionMode;

  //
  // Threshold settings
  //
  ThresholdSettings threshold;

  //
  // AI settings
  //
  AiSettings ai;

  NLOHMANN_JSON_SERIALIZE_ENUM(DetectionMode, {
                                                  {DetectionMode::NONE, ""},
                                                  {DetectionMode::THRESHOLD, "Threshold"},
                                                  {DetectionMode::AI, "Ai"},
                                              })

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(DetectionSettings, detectionMode, threshold, ai);
};

}    // namespace joda::settings
