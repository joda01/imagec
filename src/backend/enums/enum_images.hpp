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

///

#pragma once

#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/types.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

struct ImageId
{
  MemoryIdxIn imageIdx = MemoryIdxIn::$;
  joda::enums::PlaneId imagePlane;

  bool operator<(const ImageId &in) const
  {
    return imageIdx < in.imageIdx && imagePlane < in.imagePlane;
  }

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageId, imageIdx, imagePlane);
};

}    // namespace joda::enums