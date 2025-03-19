///
/// \file      enum_image_cache.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <map>
#include "backend/artifacts/image/image.hpp"
#include "backend/enums/enum_memory_idx.hpp"

namespace joda::enums {
using imageCache_t = std::map<enums::MemoryIdx, std::unique_ptr<joda::atom::ImagePlane>>;
}
