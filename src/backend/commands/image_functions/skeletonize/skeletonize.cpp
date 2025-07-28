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

void Skeletonize::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result)
{
  image.convertTo(image, CV_8UC1, 1.0F / 257.0F);
  switch(mSettings.mode) {
    case settings::SkeletonizeSettings::Mode::ZHANGSUEN:
      cv::ximgproc::thinning(image, image, cv::ximgproc::THINNING_ZHANGSUEN);
      break;
    case settings::SkeletonizeSettings::Mode::GUOHALL:
      cv::ximgproc::thinning(image, image, cv::ximgproc::THINNING_GUOHALL);
      break;
  }
  image.convertTo(image, CV_16UC1, (float) UINT16_MAX / (float) UINT8_MAX);
}

}    // namespace joda::cmd
