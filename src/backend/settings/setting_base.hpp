///
/// \file      setting_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <set>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
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
  [[nodiscard]] virtual ObjectInputClusters getInputClustersAndClasses() const
  {
    return {};
  }

  [[nodiscard]] virtual ObjectOutputClusters getOutputClustersAndClasses() const
  {
    return {};
  }

protected:
};
}    // namespace joda::settings
