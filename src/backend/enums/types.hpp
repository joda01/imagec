///
/// \file      pipeline_indexes.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <nlohmann/json.hpp>

namespace joda::enums {

using tile_t   = std::tuple<int32_t, int32_t>;
using zStack_t = int32_t;
using tStack_t = int32_t;

}    // namespace joda::enums
