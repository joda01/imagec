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
#include "../../helper/ome_parser/ome_info.hpp"
#include "../image_reader.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

auto JpgLoader::readImageMeta(const std::string &filename) -> std::string
{
  return "";
}
auto JpgLoader::getImageProperties(const std::string &filename) -> ImageProperties
{
  auto image    = cv::imread(filename, cv::IMREAD_COLOR);
  int width     = image.cols;
  int height    = image.rows;
  uint64_t size = width * height;

  return ImageProperties{.imageSize = size, .tileSize = size, .nrOfTiles = 1, .nrOfDocuments = 1};
}
cv::Mat JpgLoader::loadEntireImage(const std::string &filename)
{
  auto image       = cv::imread(filename, CV_16UC1);
  cv::Mat imageOut = cv::Mat(image.rows, image.cols, CV_16UC1);
  image.convertTo(imageOut, CV_16UC1);
  return imageOut;
}
