///
/// \file      image.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include "backend/enums/enum_images.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::atom {

class ImagePlane
{
public:
  enum class ImageType
  {
    GRAYSCALE,
    RGB,
    BINARY
  };

  void setId(const enums::ImageId &id, const enums::tile_t &tile)
  {
    this->tile = tile;
    mId        = id;
  }

  [[nodiscard]] const enums::ImageId &getId() const
  {
    return mId;
  }

  bool isBinary() const
  {
    return imageType == ImageType::BINARY;
  }

  void setToBinary()
  {
    imageType = ImageType::BINARY;
  }

  void setToGrayscale()
  {
    imageType = ImageType::GRAYSCALE;
  }

  bool isRgb() const
  {
    return image.channels() == 3;
  }

  enums::tile_t tile;
  int32_t series = 0;
  cv::Mat image;
  uint16_t appliedMinThreshold = 0;
  uint16_t appliedMaxThreshold = 0;
  enums::ImageId mId;

  ImagePlane clone(const cv::Mat &img) const
  {
    return ImagePlane{.tile                = tile,
                      .series              = series,
                      .image               = img.clone(),
                      .appliedMinThreshold = appliedMinThreshold,
                      .appliedMaxThreshold = appliedMaxThreshold,
                      .mId                 = mId,
                      .imageType           = imageType};
  }

  ImageType imageType = ImageType::GRAYSCALE;
};

}    // namespace joda::atom
