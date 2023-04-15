///
/// \file      analze_settings_parser.cpp
/// \author    Joachim Danmayr
/// \date      2023-04-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "analze_settings_parser.hpp"
#include <cctype>
#include <stdexcept>

namespace joda::settings::json {

void ChannelSettings::stringToType()
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
  } else if(type == "CELL_BRIGHTFIELD") {
    enumType = Type::CELL_BRIGHTFIELD;
  } else if(type == "CELL_DARKFIELD") {
    enumType = Type::CELL_DARKFIELD;
  } else {
    throw std::runtime_error("Channel type >" + type + "< is not a valid setting!");
  }
}

void ChannelSettings::stringToThreshold()
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
  } else {
    throw std::runtime_error("Threshold >" + threshold_algorithm + "< is not a valid setting!");
  }
}

void ChannelSettings::stringToZProjection()
{
  std::transform(zprojection.begin(), zprojection.end(), zprojection.begin(),
                 [](unsigned char c) { return std::toupper(c); });

  if(zprojection == "NONE" || zprojection.empty()) {
    enumZProjection = ZProjection::NONE;
  } else if(zprojection == "MAX") {
    enumZProjection = ZProjection::MAX_INTENSITY;
  } else {
    throw std::runtime_error("ZProjection >" + zprojection + "< is not a valid setting!");
  }
}

void AnalyzeSettings::stringToPipeline()
{
  std::transform(pipeline.begin(), pipeline.end(), pipeline.begin(), [](unsigned char c) { return std::toupper(c); });

  if(pipeline == "NUCLEUS_COUNT") {
    enumPipeline = Pipeline::NUCLEUS_COUNT;
  } else if(pipeline == "EV_COUNT") {
    enumPipeline = Pipeline::EV_COUNT;
  } else if(pipeline == "EV_COLOC") {
    enumPipeline = Pipeline::EV_COLOC;
  } else if(pipeline == "EV_COLOC_IN_CELLS") {
    enumPipeline = Pipeline::EV_COLOC_IN_CELL;
  } else {
    throw std::runtime_error("Pipeline >" + pipeline + "< is not a valid pipeline function!");
  }
}

}    // namespace joda::settings::json
