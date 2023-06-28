///
/// \file      channel_settings_parser.cpp
/// \author    Joachim Danmayr
/// \date      2023-06-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "channel_settings.hpp"

namespace joda::settings::json {

void ChannelInfo::stringToType()
{
  std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c) { return std::toupper(c); });

  if(type == "NONE" || type.empty()) {
    enumType = Type::NONE;
  } else if(type == "NUCLEUS") {
    enumType = Type::NUCLEUS;
  } else if(type == "EV") {
    enumType = Type::EV;
  } else if(type == "BACKGROUND") {
    enumType = Type::BACKGROUND;
  } else if(type == "CELL") {
    enumType = Type::CELL;
  } else {
    throw std::runtime_error("Channel type >" + type + "< is not a valid setting!");
  }
}

void ChannelDetection::stringToDetectionMode()
{
  std::transform(mode.begin(), mode.end(), mode.begin(), [](unsigned char c) { return std::toupper(c); });

  if(mode == "THRESHOLD") {
    enumDetectionMode = DetectionMode::THRESHOLD;
  } else if(mode == "AI") {
    enumDetectionMode = DetectionMode::AI;

  } else {
    throw std::runtime_error("Detection mode >" + mode + "< is not a valid detection mode!");
  }
}
}    // namespace joda::settings::json
