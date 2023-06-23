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
#include <string>
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
public:
  Image(cv::Mat image, std::string name, int64_t channelNr, int64_t tileNr) :
      mImage(image), mName(name), mChannelNr(channelNr), mTileNr(tileNr)
  {
  }

  auto getUniqueName() const -> std::string
  {
    if(mTileNr >= 0) {
      return mName + "_" + std::to_string(mTileNr) + "_" + std::to_string(mChannelNr);
    }
    return mName + "_" + std::to_string(mChannelNr);
  }

  auto name() const -> const std::string &
  {
    return mName;
  }

  cv::Mat mImage;

private:
  std::string mName;
  int64_t mChannelNr = -1;
  int64_t mTileNr    = -1;
};
}    // namespace joda
