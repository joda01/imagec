///
/// \file      bioformats.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once
#include <iostream>
#include <string>
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

class BioformatsLoader
{
public:
  static cv::Mat loadEntireImage(const std::string &filename, int directory);
  static auto getOmeInformation(const std::string &filename) -> std::tuple<joda::ome::OmeInfo, ImageProperties>;
};
