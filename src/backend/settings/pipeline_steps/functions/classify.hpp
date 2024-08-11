#pragma once

#include <cstdint>
#include <set>
#include "backend/helper/json_optional_parser_helper.hpp"
#include "backend/settings/pipeline_steps/store_slot.hpp"
#include "classify/classify_ai.hpp"
#include "classify/classify_threshold.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

class Classify
{
public:
  //
  // Use either threshold or AI for classification
  //
  std::optional<std::map<int32_t, ThresholdSettings>> $threshold;
  std::optional<AiSettings> $ai;

  //
  // Slots where the classified objects should be stored in.
  // The key of the map is the class identifier defined by the AI model or
  // in case of thersholding the id given in the threshold map.
  //
  std::map<int32_t, StoreSlot> store;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Classify, $threshold, $ai, store);
};

}    // namespace joda::settings
