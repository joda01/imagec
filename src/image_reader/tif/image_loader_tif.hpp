

#pragma once

#include <string>
#include <opencv2/core/mat.hpp>

class TiffLoader
{
public:
  struct ImageProperties
  {
    int64_t imageSize      = 0;
    int64_t tileSize       = 0;
    int64_t nrOfTiles      = 0;
    uint16_t nrOfDocuments = 0;
    std::string oem;
  };

  static void initLibTif();
  static cv::Mat loadImageTile(const std::string &filename, uint16_t directory, int offset, int nrOfTilesToRead);
  static auto readImageMeta(const std::string &filename) -> std::string;
  static auto getImageProperties(const std::string &filename, uint16_t directory) -> ImageProperties;
  static cv::Mat loadEntireImage(const std::string &filename, int directory);
};
