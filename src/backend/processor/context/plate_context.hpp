///
/// \file      plate_context.hpp
/// \author
/// \date      2024-08-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <filesystem>
#include "backend/artifacts/image/image.hpp"
#include "backend/enums/enums_clusters.hpp"

namespace joda::processor {

struct PlateContext
{
  uint16_t plateId;
};

}    // namespace joda::processor
