///
/// \file      image_loader_jpg.cpp
/// \author    Joachim Danmayr
/// \date      2023-06-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "image_loader_jpg.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::image {

auto JpgLoader::readImageMeta(const std::string &filename) -> std::string
{
  return "";
}
auto JpgLoader::getImageProperties(const std::string &filename) -> joda::ome::OmeInfo
{
  auto image     = cv::imread(filename, cv::IMREAD_COLOR);
  int64_t width  = image.cols;
  int64_t height = image.rows;
  int64_t size   = width * height;
  joda::ome::OmeInfo omeInfo;
  omeInfo.emulateOmeInformationFromTiff(
      joda::ome::OmeInfo::ImageInfo{.seriesIdx      = 0,
                                    .nrOfChannels   = 1,
                                    .bits           = 16,
                                    .tileNrOfPixels = size,
                                    .tileWidth      = width,
                                    .tileHeight     = height,
                                    .nrOfDocuments  = 1,
                                    .resolutions    = {{0, joda::ome::OmeInfo::ImageInfo::Pyramid{
                                                               .imageMemoryUsage = 0,
                                                               .imageNrOfPixels  = size,
                                                               .imageWidth       = width,
                                                               .imageHeight      = height,
                                                               .tileNr           = 1,
                                                        }}}});
  return omeInfo;
}
cv::Mat JpgLoader::loadEntireImage(const std::string &filename)
{
  auto image       = cv::imread(filename, CV_16UC1);
  cv::Mat imageOut = cv::Mat(image.rows, image.cols, CV_16UC1);
  image.convertTo(imageOut, CV_16UC1);
  return imageOut;
}
}    // namespace joda::image
