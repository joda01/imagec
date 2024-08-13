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

#include "backend/enums/types.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

enum class ImageIdx : uint16_t
{
  I0 = 0,
  I1 = 1,
  I2 = 2,
  $  = 0xFFFF
};

struct ImageId
{
  ImageIdx imageIdx                 = ImageIdx::I0;
  joda::enums::IteratorId iteration = {-1, -1, -1};

  bool operator<(const ImageId &in) const
  {
    return imageIdx < in.imageIdx && iteration < in.iteration;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImageId, imageIdx, iteration);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ImageIdx,
                             {{ImageIdx::$, "$"}, {ImageIdx::I0, "I0"}, {ImageIdx::I1, "I1"}, {ImageIdx::I2, "I2"}});

}    // namespace joda::enums
