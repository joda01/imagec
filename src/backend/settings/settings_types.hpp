///
/// \file      settings_types.hpp
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

#include "backend/enums/enums_classes.hpp"
#include "setting.hpp"

namespace joda::settings {

using ObjectOutputClasses    = std::set<enums::ClassIdIn>;
using ObjectInputClasses     = std::set<enums::ClassIdIn>;
using ObjectInputClasss      = enums::ClassIdIn;
using ObjectOutputClasss     = enums::ClassIdIn;
using ObjectInputClassesExp  = std::set<enums::ClassId>;
using ObjectOutputClassesExp = std::set<enums::ClassId>;

}    // namespace joda::settings
