///
/// \file      random_forest_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"

namespace joda::settings {

struct RandomForestSettings : public SettingBase
{
  //
  // ID of the classes to train
  //
  joda::enums::ClassIdIn trainingClasses = joda::enums::ClassIdIn::UNDEFINED;
};

}    // namespace joda::settings
