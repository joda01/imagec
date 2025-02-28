///
/// \file      enum_history.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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
