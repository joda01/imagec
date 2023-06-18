

#pragma once

#include <string>
#include "../image_reader.hpp"
#include <opencv2/core/mat.hpp>

class JpgLoader
{
public:
  static auto readImageMeta(const std::string &filename) -> std::string;
  static auto getImageProperties(const std::string &filename, uint16_t directory) -> ImageProperties;
  static cv::Mat loadEntireImage(const std::string &filename, int directory);
};
