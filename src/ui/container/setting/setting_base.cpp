///
/// \file      container_function.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "setting_base.hpp"

namespace joda::ui::qt {

SettingBase::SettingBase()
{
}

void SettingBase::triggerValueChanged()
{
  emit valueChanged();
}

}    // namespace joda::ui::qt
