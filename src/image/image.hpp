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
class Image
{
public:
  /////////////////////////////////////////////////////
  static constexpr uint32_t TYPE_MASK = 0xFF000000;

  /////////////////////////////////////////////////////
  enum class Type : uint32_t
  {
    UNDEFINED  = 0x00000000,
    EV         = 0x01000000,
    CELL       = 0x02000000,
    NUCLUES    = 0x03000000,
    BACKGROUND = 0x04000000,
  };

  enum class Channel : uint32_t
  {
    CELL       = (uint32_t) Type::CELL,
    NUCLUES    = (uint32_t) Type::NUCLUES,
    BACKGROUND = (uint32_t) Type::BACKGROUND,
    EV_DAPI    = (uint32_t) Type::EV | 1,
    EV_CY3     = (uint32_t) Type::EV | 2,
    EV_CY5     = (uint32_t) Type::EV | 3,
  };

  /////////////////////////////////////////////////////
  Image();
  void addImage(Channel ch, std::shared_ptr<cv::Mat> img);
  auto getImage(Channel ch) -> std::shared_ptr<cv::Mat>;
  auto getAllOfType(Type t) -> std::map<Channel, std::shared_ptr<cv::Mat>>;

private:
  /////////////////////////////////////////////////////
  std::map<Type, std::map<Channel, std::shared_ptr<cv::Mat>>> mChannels;
};
}    // namespace joda
