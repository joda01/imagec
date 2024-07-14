

#pragma once

#include <string>
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::image {

class JpgLoader
{
public:
  static auto readImageMeta(const std::string &filename) -> std::string;
  static auto getImageProperties(const std::string &filename) -> joda::ome::OmeInfo;
  static cv::Mat loadEntireImage(const std::string &filename);
};
}    // namespace joda::image
