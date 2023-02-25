///
/// \file      image.cpp
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

#include "image.hpp"
#include <map>

namespace joda {

Image::Image()
{
}

void Image::addImage(Channel ch, std::shared_ptr<cv::Mat> img)
{
  Type t           = (Type) ((uint32_t) ch & TYPE_MASK);
  mChannels[t][ch] = img;
}

auto Image::getImage(Channel ch) -> std::shared_ptr<cv::Mat>
{
  Type t         = (Type) ((uint32_t) ch & TYPE_MASK);
  auto foundType = mChannels.find(t);
  if(foundType != mChannels.end()) {
    auto channel = foundType->second.find(ch);
    if(channel != foundType->second.end()) {
      return channel->second;
    }
  }
  // Not found
  return nullptr;
}

auto Image::getAllOfType(Type t) -> std::map<Channel, std::shared_ptr<cv::Mat>>
{
  auto foundType = mChannels.find(t);
  if(foundType != mChannels.end()) {
    return foundType->second;
  }
  // Not found
  return std::map<Channel, std::shared_ptr<cv::Mat>>();
}

}    // namespace joda
