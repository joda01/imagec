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

#include "container_function.hpp"

namespace joda::ui::qt {

ContainerFunctionBase::ContainerFunctionBase()
{
}

void ContainerFunctionBase::triggerValueChanged()
{
  emit valueChanged();
}

}    // namespace joda::ui::qt
