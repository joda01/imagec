

#pragma once

#include <string>
#include "../image_reader.hpp"
#include "helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

class TiffLoader
{
public:
  static void initLibTif();
  static cv::Mat loadImageTile(const std::string &filename, uint16_t directory, int offset, int nrOfTilesToRead);
  static auto getOmeInformation(const std::string &filename) -> joda::ome::OmeInfo;
  static auto getImageProperties(const std::string &filename, uint16_t directory) -> ImageProperties;
  static cv::Mat loadEntireImage(const std::string &filename, int directory);
};
