
#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/anaylze_settings_enums.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

class StoreSlot
{
public:
  //
  // The ID of the slot the result should be stored in
  //
  Slot slotId;

  //
  // The class ID which should be assigned to this slot
  // Use "$" to use the class ID of the actual slot
  //
  ObjectClassId $classId;

  //
  // An optional human readable name
  //
  std::string name;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(StoreSlot, slotId, $classId, name);
};
}    // namespace joda::settings
