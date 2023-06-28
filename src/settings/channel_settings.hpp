///
/// \file      channel_settings.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <cstddef>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <vector>
#include <catch2/catch_config.hpp>
#include <nlohmann/json.hpp>
#include "preprocessing_settings.hpp"

namespace joda::settings::json {

class AiSettings final
{
public:
  /////////////////////////////////////////////////////
  auto getModelName() const -> std::string
  {
    return model_name;
  }

private:
  //
  // Name of the onnx AI model which should be used for detection.
  //
  std::string model_name;
  float probability_min;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AiSettings, model_name, probability_min);
};

class ThresholdSettings final
{
public:
  /////////////////////////////////////////////////////
  enum class Threshold
  {
    NONE,
    MANUAL,
    LI,
    MIN_ERROR,
    TRIANGLE
  };

  void interpretConfig()
  {
    stringToThreshold();
  }

  auto getThreshold() const -> Threshold
  {
    return enumThreshold;
  }

  auto getThresholdMin() const -> float
  {
    return threshold_min;
  }

  auto getThresholdMax() const -> float
  {
    return threshold_max;
  }

private:
  //
  // Which threshold algorithm should be used
  // [MANUAL, LI, MIN_ERROR, TRIANGLE]
  //
  std::string threshold_algorithm;
  Threshold enumThreshold = Threshold::NONE;
  void stringToThreshold();

  //
  // Minimum threshold value.
  // [0-65535]
  //
  float threshold_min = 0;

  //
  // Maximum threshold value (default 65535)
  // [0-65535]
  //
  float threshold_max = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ThresholdSettings, threshold_algorithm, threshold_min, threshold_max);
};

class ChannelDetection final
{
public:
  enum class DetectionMode
  {
    NONE,
    THRESHOLD,
    AI
  };

  auto getDetectionMode() const -> const DetectionMode &
  {
    return enumDetectionMode;
  }

  auto getThersholdSettings() const -> const ThresholdSettings &
  {
    return thresholds;
  }

  auto getAiSettings() const -> const AiSettings &
  {
    return ai_settings;
  }

  void interpretConfig()
  {
    stringToDetectionMode();
    thresholds.interpretConfig();
    // ai_settings.interpretConfig();
  }

private:
  //
  // If either threshold or AI should be used for detection
  //
  std::string detection_mode;
  DetectionMode enumDetectionMode;
  void stringToDetectionMode();

  //
  // Threshold settings
  //
  ThresholdSettings thresholds;

  //
  // AI settings
  //
  AiSettings ai_settings;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelDetection, detection_mode, thresholds, ai_settings);
};

class ChannelInfo
{
public:
  /////////////////////////////////////////////////////
  enum class Type
  {
    NONE,
    NUCLEUS,
    EV,
    CELL,
    BACKGROUND,
  };

  auto getChannelIndex() const -> uint32_t
  {
    return index;
  }

  auto getType() const -> Type
  {
    return enumType;
  }

  auto getLabel() const -> std::string
  {
    return label;
  }

  void interpretConfig()
  {
    stringToType();
  }

private:
  /////////////////////////////////////////////////////
  //
  // Corresponding channel
  // [0, 1, 2, ...]
  //
  uint32_t index;

  //
  // User defined name of the channel
  //
  std::string name;

  //
  // What is seen in this channel
  // [NUCLEUS, EV, BACKGROUND, CELL_BRIGHTFIELD, CELL_DARKFIELD]
  //
  std::string type;
  Type enumType;
  void stringToType();

  //
  // Label of the channel if available
  // [CY5, CY3]
  //
  std::string label;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelInfo, index, name, type, label);
};

class ChannelFiltering final
{
public:
  auto getMinParticleSize() const -> float
  {
    return min_particle_size;
  }

  auto getMaxParticleSize() const -> float
  {
    return max_particle_size;
  }

  auto getMinCircularity() const -> float
  {
    return min_circularity;
  }

  auto getSnapAreaSize() const -> float
  {
    return snap_area_size;
  }

  /////////////////////////////////////////////////////
private:
  //
  // Every particle with a diameter lower than that is ignored during analysis.
  // Value in [px]
  //
  float min_particle_size;

  //
  // Every particle with a diameter bigger than that is ignored during analysis.
  // Value in [px]
  //
  float max_particle_size;

  //
  // Every particle with a circularity lower than this value is ignored during analysis.
  // Value in [0-1]
  //
  float min_circularity;

  //
  // Used for coloc algorithm to define a tolerance around each particle.
  // Value in [px]
  //
  float snap_area_size;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelFiltering, min_particle_size, max_particle_size, min_circularity,
                                              snap_area_size);
};

class ChannelSettings final
{
public:
  /////////////////////////////////////////////////////

  enum class ZProjection
  {
    NONE,
    MAX_INTENSITY
  };

  /////////////////////////////////////////////////////
  void interpretConfig()
  {
    stringToZProjection();
    info.interpretConfig();
    detection.interpretConfig();

    for(auto &pre : preprocessing) {
      pre.interpretConfig();
    }
  }

  auto getChannelInfo() const -> const ChannelInfo &
  {
    return info;
  }

  auto getDetectionSettings() const -> const ChannelDetection &
  {
    return detection;
  }

  auto getFilter() const -> const ChannelFiltering &
  {
    return filter;
  }

  auto getMarginCrop() const -> float
  {
    return margin_crop;
  }

  auto getZProjection() const -> ZProjection
  {
    return enumZProjection;
  }

private:
  //
  // How much of the edge should be cut off.
  // Value in [px]
  //
  float margin_crop;

  //
  // Do a z-projection before analysis starts
  // [NONE, MAX]
  //
  std::string zprojection;
  ZProjection enumZProjection;
  void stringToZProjection();

  //
  // Common channel information
  //
  ChannelInfo info;

  std::vector<PreprocessingStep> preprocessing;

  //
  // Detection settings
  //
  ChannelDetection detection;

  //
  // Filter for the channel which are applied after detection
  //
  ChannelFiltering filter;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettings, info, detection, preprocessing, filter, margin_crop,
                                              zprojection);
};

}    // namespace joda::settings::json
