///
/// \file      interation_context.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <filesystem>
#include "backend/artifacts/object_list/object_list.hpp"

namespace joda::processor {

struct IterationContext
{
  joda::atom::ObjectList actObjects{};
};

}    // namespace joda::processor
