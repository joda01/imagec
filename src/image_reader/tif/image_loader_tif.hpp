

#pragma once

#include <string>
#include <opencv2/core/mat.hpp>

class TiffLoader
{
public:
  static cv::Mat loadImageTile(const std::string &filename, int document, int offset, int nrOfTilesToRead = 100);
  static cv::Mat loadEntireImage(const std::string &filename, int directory);
};
