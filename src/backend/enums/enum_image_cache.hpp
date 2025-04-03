///
/// \file      enum_image_cache.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <map>
#include "backend/artifacts/image/image.hpp"
#include "backend/enums/enum_memory_idx.hpp"

namespace joda::enums {
using imageCache_t = std::map<enums::MemoryIdx, std::unique_ptr<joda::atom::ImagePlane>>;
}
