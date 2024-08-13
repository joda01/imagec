///
/// \file      image.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/enums/enum_images.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::atom {

class Image
{
public:
  void setId(const enums::ImageId &id, const enums::tile_t &tile)
  {
    this->tile = tile;
    mId        = id;
  }

  [[nodiscard]] const enums::ImageId &getId() const
  {
    return mId;
  }
  enums::tile_t tile;
  cv::Mat image;
  bool isBinary                = false;
  uint16_t appliedMinThreshold = 0;
  uint16_t appliedMaxThreshold = 0;

private:
  enums::ImageId mId;
};

}    // namespace joda::atom
