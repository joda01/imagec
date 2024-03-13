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

  auto getProbability() const -> float
  {
    return probability_min;
  }

private:
  //
  // Name of the onnx AI model which should be used for detection.
  //
  std::string model_name;
  float probability_min = 0;

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
    TRIANGLE,
    MOMENTS,
    HUANG,
    INTERMODES,
    ISODATA,
    MAX_ENTROPY,
    MEAN,
    MINIMUM,
    OTSU,
    PERCENTILE,
    RENYI_ENTROPY,
    SHANBHAG,
    YEN
  };

  void interpretConfig()
  {
    stringToThreshold();
  }

  auto getThreshold() const -> Threshold
  {
    return enumThreshold;
  }

  auto getThresholdString() const -> const std::string &
  {
    return threshold_algorithm;
  }

  auto getThresholdMin() const -> uint16_t
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
  uint16_t threshold_min = 0;

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

  auto getDetectionModeString() const -> const std::string &
  {
    return mode;
  }

  auto getThersholdSettings() const -> const ThresholdSettings &
  {
    return threshold;
  }

  auto getAiSettings() const -> const AiSettings &
  {
    return ai_settings;
  }

  void interpretConfig()
  {
    stringToDetectionMode();
    threshold.interpretConfig();
    // ai_settings.interpretConfig();
  }

private:
  //
  // If either threshold or AI should be used for detection
  //
  std::string mode;
  DetectionMode enumDetectionMode;
  void stringToDetectionMode();

  //
  // Threshold settings
  //
  ThresholdSettings threshold;

  //
  // AI settings
  //
  AiSettings ai_settings;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelDetection, mode, threshold, ai_settings);
};

class ChannelInfo
{
public:
  /////////////////////////////////////////////////////
  enum class Type
  {
    NONE,
    SPOT,
    SPOT_REFERENCE,
    NUCLEUS,
    CELL,
    BACKGROUND,
  };

  auto getChannelIndex() const -> int32_t
  {
    return index;
  }

  auto getChannelSeries() const -> int32_t
  {
    return series;
  }

  auto getType() const -> Type
  {
    return enumType;
  }

  auto getTypeString() const -> const std::string &
  {
    return type;
  }

  auto getLabel() const -> const std::string &
  {
    return label;
  }

  auto getColor() const -> const std::string &
  {
    return color;
  }

  auto getName() const -> const std::string &
  {
    if(name.empty()) {
      return type;
    }
    return name;
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
  int32_t index;

  //
  // Series to analyze
  // [0, 1, 2, ...]
  //
  int32_t series = 0;

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

  //
  // Color of the channel
  // [#FFFFFFFF]
  //
  std::string color = "#000000";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelInfo, index, series, name, type, label, color);
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

  auto getReferenceSpotChannelIndex() const -> int32_t
  {
    return reference_spot_channel_index;
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

  //
  // Index of the reference spot channel.
  // If bigger than 0 this channel will be used to remove reference spots from the channel
  //
  int32_t reference_spot_channel_index = -1;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelFiltering, min_particle_size, max_particle_size, min_circularity,
                                              snap_area_size, reference_spot_channel_index);
};

class ChannelSettings final
{
public:
  /////////////////////////////////////////////////////

  void loadConfigFromString(const std::string &jsonString)
  {
    *this = nlohmann::json::parse(jsonString);
    interpretConfig(-1);
  }

  void loadConfigFromFile(const std::string &cfgPath)
  {
    std::ifstream input(cfgPath);
    std::ostringstream oss;
    oss << input.rdbuf();    // Read the file buffer into the ostringstream
    std::string read = oss.str();
    *this            = nlohmann::json::parse(read);
    interpretConfig(-1);
    input.close();
  }

  void interpretConfig(int arrayIndex)
  {
    mArrayIndex = arrayIndex;
    info.interpretConfig();
    detection.interpretConfig();

    for(auto &pre : preprocessing) {
      pre.interpretConfig();
      auto method = pre.getZStack();
      if(method) {
        zStackMethod = method->getZStackMethod();
      }

      auto method2 = pre.getSubtractChannel();
      if(method2) {
        mSubtractChannel = method2->channel_index;
      }
    }
  }

  auto getArrayIndex() const -> int32_t
  {
    return mArrayIndex;
  }

  auto getChannelInfo() const -> const ChannelInfo &
  {
    return info;
  }

  auto getDetectionSettings() const -> const ChannelDetection &
  {
    return detection;
  }

  auto getPreprocessingFunctions() const -> const std::vector<PreprocessingStep> &
  {
    return preprocessing;
  }

  auto getZProjectionSetting() const -> PreprocessingZStack::ZStackMethod
  {
    return zStackMethod;
  }

  auto getPreprocessingSubtractChannel() const -> int32_t
  {
    return mSubtractChannel;
  }

  auto getFilter() const -> const ChannelFiltering &
  {
    return filter;
  }

private:
  //
  // Common channel information
  //
  ChannelInfo info;

  //
  // Preprocessing steps
  //
  std::vector<PreprocessingStep> preprocessing;
  PreprocessingZStack::ZStackMethod zStackMethod = PreprocessingZStack::ZStackMethod::NONE;
  int32_t mSubtractChannel                       = -1;

  //
  // Detection settings
  //
  ChannelDetection detection;

  //
  // Filter for the channel which are applied after detection
  //
  ChannelFiltering filter;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettings, info, detection, preprocessing, filter);

  //
  // For internal needs. Not stored in the json: Actual index in the channel array
  //
  int32_t mArrayIndex = -1;
};

}    // namespace joda::settings::json
