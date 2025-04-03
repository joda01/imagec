///
/// \file      enum_history.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <nlohmann/json.hpp>

namespace joda::enums {

enum class HistoryCategory
{
  OTHER,
  ADDED,
  DELETED,
  CHANGED,
  SAVED
};

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(HistoryCategory, {
                                                  {HistoryCategory::OTHER, "Other"},
                                                  {HistoryCategory::ADDED, "Added"},
                                                  {HistoryCategory::DELETED, "Deleted"},
                                                  {HistoryCategory::CHANGED, "Changed"},
                                                  {HistoryCategory::SAVED, "Saved"},
                                              });

}    // namespace joda::enums
