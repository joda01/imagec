///
/// \file      image_reader.hpp
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

#pragma once

#include "helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

struct ImageProperties
{
  int64_t imageSize      = 0;
  int64_t tileSize       = 0;
  int64_t nrOfTiles      = 0;
  uint16_t nrOfDocuments = 0;
  joda::ome::OmeInfo omeInformation;
};
