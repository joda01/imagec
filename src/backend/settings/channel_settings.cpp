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
  } else if(type == "SPOT") {
    enumType = Type::SPOT;
  } else if(type == "SPOT_REFERENCE") {
    enumType = Type::SPOT_REFERENCE;
  } else if(type == "BACKGROUND") {
    enumType = Type::BACKGROUND;
  } else if(type == "CELL") {
    enumType = Type::CELL;
  } else {
    throw std::runtime_error("Channel type >" + type + "< is not a valid setting!");
  }
}

void ThresholdSettings::stringToThreshold()
{
  std::transform(threshold_algorithm.begin(), threshold_algorithm.end(), threshold_algorithm.begin(),
                 [](unsigned char c) { return std::toupper(c); });

  if(threshold_algorithm == "MANUAL") {
    enumThreshold = Threshold::MANUAL;
  } else if(threshold_algorithm == "LI") {
    enumThreshold = Threshold::LI;
  } else if(threshold_algorithm == "MIN_ERROR") {
    enumThreshold = Threshold::MIN_ERROR;
  } else if(threshold_algorithm == "TRIANGLE") {
    enumThreshold = Threshold::TRIANGLE;
  } else if(threshold_algorithm == "MOMENTS") {
    enumThreshold = Threshold::MOMENTS;
  } else if(threshold_algorithm == "HUANG") {
    enumThreshold = Threshold::HUANG;
  } else if(threshold_algorithm == "INTERMODES") {
    enumThreshold = Threshold::INTERMODES;
  } else if(threshold_algorithm == "ISODATA") {
    enumThreshold = Threshold::ISODATA;
  } else if(threshold_algorithm == "MAX_ENTROPY") {
    enumThreshold = Threshold::MAX_ENTROPY;
  } else if(threshold_algorithm == "MEAN") {
    enumThreshold = Threshold::MEAN;
  } else if(threshold_algorithm == "MINIMUM") {
    enumThreshold = Threshold::MINIMUM;
  } else if(threshold_algorithm == "OTSU") {
    enumThreshold = Threshold::OTSU;
  } else if(threshold_algorithm == "PERCENTILE") {
    enumThreshold = Threshold::PERCENTILE;
  } else if(threshold_algorithm == "RENYI_ENTROPY") {
    enumThreshold = Threshold::RENYI_ENTROPY;
  } else if(threshold_algorithm == "SHANBHAG") {
    enumThreshold = Threshold::SHANBHAG;
  } else if(threshold_algorithm == "YEN") {
    enumThreshold = Threshold::YEN;
  } else {
    throw std::runtime_error("Threshold >" + threshold_algorithm + "< is not a valid setting!");
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
