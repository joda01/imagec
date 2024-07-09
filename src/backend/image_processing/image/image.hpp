///
/// \file      image.hpp
/// \author    Joachim Danmayr
/// \date      2024-07-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qpixmap.h>
#include <vector>
#include <opencv2/core/mat.hpp>

namespace joda::image {

///
/// \class      Image
/// \author     Joachim Danmayr
/// \brief      Class representing an image instance
///
class Image
{
public:
  /////////////////////////////////////////////////////
  Image();
  void setImage(const cv::Mat &imageToDisplay);
  bool empty()
  {
    return mImageOriginal.empty();
  }
  [[nodiscard]] const cv::Mat &getImage() const
  {
    return mImageOriginal;
  }
  [[nodiscard]] const QPixmap &getPixmap() const
  {
    return mPixmap;
  }

private:
  /////////////////////////////////////////////////////
  std::vector<uchar> encodeToPNG();
  void toPixmap();

  /////////////////////////////////////////////////////
  cv::Mat mImageOriginal;
  cv::Mat mEditedImage;
  QPixmap mPixmap;
};

}    // namespace joda::image
