///
/// \file      intensity.cpp
/// \author    Joachim Danmayr
/// \date      2024-09-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "intensity.hpp"
#include "backend/commands/image_functions/intensity/intensity_settings.hpp"

namespace joda::cmd {

void IntensityTransformation::execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*result*/)
{
  if(mSettings.mode == settings::IntensityTransformationSettings::Mode::AUTOMATIC) {
    image = equalizeHist16Bit(image);
  } else {
    // Modify contrast and brightness for 16-bit image
    for(int y = 0; y < image.rows; y++) {
      for(int x = 0; x < image.cols; x++) {
        image.at<uint16_t>(y, x) =
            cv::saturate_cast<uint16_t>(mSettings.contrast * static_cast<float>(image.at<uint16_t>(y, x)) + static_cast<float>(mSettings.brightness));
      }
    }
  }
}

cv::Mat IntensityTransformation::equalizeHist16Bit(const cv::Mat &src)
{
  CV_Assert(src.type() == CV_16UC1);    // Ensure it's a 16-bit single-channel image

  // Step 1: Calculate the histogram (65536 bins for 16-bit range)
  std::vector<int> hist(65536, 0);
  for(int i = 0; i < src.rows; ++i) {
    const uint16_t *row = src.ptr<uint16_t>(i);
    for(int j = 0; j < src.cols; ++j) {
      hist[row[j]]++;
    }
  }

  // Step 2: Calculate the cumulative distribution function (CDF)
  std::vector<int> cdf(65536, 0);
  cdf[0] = hist[0];
  for(int i = 1; i < 65536; ++i) {
    cdf[i] = cdf[i - 1] + hist[i];
  }

  // Normalize the CDF to the range [0, 65535]
  int total_pixels = src.rows * src.cols;
  std::vector<uint16_t> equalization_map(65536);
  for(int i = 0; i < 65536; ++i) {
    equalization_map[i] = static_cast<uint16_t>(65535.0 * cdf[i] / total_pixels);
  }

  // Step 3: Create the output image and map the pixel values
  cv::Mat dst = src.clone();
  for(int i = 0; i < src.rows; ++i) {
    auto *dst_row       = dst.ptr<uint16_t>(i);
    const auto *src_row = src.ptr<uint16_t>(i);
    for(int j = 0; j < src.cols; ++j) {
      dst_row[j] = equalization_map[src_row[j]];
    }
  }

  return dst;
}

}    // namespace joda::cmd
