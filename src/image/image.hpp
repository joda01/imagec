///
/// \file      image.hpp
/// \author    Joachim Danmayr
/// \date      2023-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     An multi channel image representation
///

#pragma once

#include <map>
#include <memory>
#include <vector>
#include <opencv2/core/mat.hpp>

namespace joda {

///
/// \class      Image
/// \author     Joachim Danmayr
/// \brief      Multi channel image
///
struct Image
{
  cv::Mat mImage;
  std::string mName;
};
}    // namespace joda
