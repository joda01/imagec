///
/// \file      Skeletonize.cpp
/// \author
/// \date      2023-10-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     C++ implementation of Skeletonize algorithm based on
///            ImageJ short processor implementation
///
/// \ref       https://github.com/imagej/ImageJ/blob/master/ij/process/ShortProcessor.java
///

#include "skeletonize.hpp"
#include "backend/commands/image_functions/skeletonize/skeletonize_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/ximgproc.hpp>

namespace joda::cmd {

void Skeletonize::execute(cv::Mat &image)
{
  image.convertTo(image, CV_8UC1, 1.0 / 257.0);
  switch(mSettings.mode) {
    case settings::SkeletonizeSettings::Mode::ZHANGSUEN:
      cv::ximgproc::thinning(image, image, cv::ximgproc::THINNING_ZHANGSUEN);
      break;
    case settings::SkeletonizeSettings::Mode::GUOHALL:
      cv::ximgproc::thinning(image, image, cv::ximgproc::THINNING_GUOHALL);
      break;
  }
  image.convertTo(image, CV_16UC1, static_cast<double>(UINT16_MAX) / static_cast<double>(UINT8_MAX));
}

}    // namespace joda::cmd
