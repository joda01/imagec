///
/// \file      setting_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <set>
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enums_classes.hpp"

#include "backend/settings/settings_types.hpp"

namespace joda::settings {

///
/// \class      SettingBase
/// \author     Joachim Danmayr
/// \brief
///
class SettingBase
{
public:
  SettingBase() = default;
  [[nodiscard]] virtual ObjectInputClasses getInputClasses() const
  {
    return {};
  }

  [[nodiscard]] virtual ObjectOutputClasses getOutputClasses() const
  {
    return {};
  }

  [[nodiscard]] virtual std::set<enums::MemoryIdx::Enum> getInputImageCache() const
  {
    return {};
  }

  [[nodiscard]] virtual std::set<enums::MemoryIdx::Enum> getOutputImageCache() const
  {
    return {};
  }

protected:
};
}    // namespace joda::settings
