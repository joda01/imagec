///
/// \file      store_to_db_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <cstdint>
#include <set>
#include <vector>
#include "backend/commands/setting.hpp"
#include "backend/enums/enums_slot_channels.hpp"
#include "backend/global_enums.hpp"
#include <nlohmann/json.hpp>

namespace joda::cmd::functions {

struct StoreToDbSettings : public Setting
{
  joda::enums::Slot slotToStoreDataUnder = joda::enums::Slot::$;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(StoreToDbSettings, slotToStoreDataUnder);
};

}    // namespace joda::cmd::functions
