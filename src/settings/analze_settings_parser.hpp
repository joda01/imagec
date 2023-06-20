///
/// \file      analyze_settings.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-03
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
#include <catch2/catch_config.hpp>
#include <nlohmann/json.hpp>

namespace joda::settings::json {

using json = nlohmann::json;

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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AiSettings, model_name);
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

class ChannelSettings final
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

  enum class ZProjection
  {
    NONE,
    MAX_INTENSITY
  };

  enum class DetectionMode
  {
    NONE,
    THRESHOLD,
    AI
  };

  /////////////////////////////////////////////////////
  void interpretConfig()
  {
    stringToType();
    stringToZProjection();
    stringToDetectionMode();
    thresholds.interpretConfig();
  }

  auto getChannelIndex() const -> uint32_t
  {
    return index;
  }

  auto getType() const -> Type
  {
    return enumType;
  }

  auto getDetectionMode() const -> DetectionMode
  {
    return enumDetectionMode;
  }

  auto getLabel() const -> std::string
  {
    return label;
  }

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

  auto getMarginCrop() const -> float
  {
    return margin_crop;
  }

  auto getZProjection() const -> ZProjection
  {
    return enumZProjection;
  }

  auto getThersholdSettings() const -> std::optional<ThresholdSettings>
  {
    return thresholds;
  }

  auto getAiSettings() const -> std::optional<AiSettings>
  {
    return ai_settings;
  }

private:
  //
  // Corresponding channel
  // [0, 1, 2, ...]
  //
  uint32_t index;

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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettings, index, type, label, detection_mode, thresholds,
                                              ai_settings, min_particle_size, max_particle_size, min_circularity,
                                              snap_area_size, margin_crop, zprojection);
};

class AnalyzeSettings final
{
public:
  /////////////////////////////////////////////////////
  enum class Pipeline
  {
    COUNT,
    COLOC,
    COLOC_IN_CELL
  };

  /////////////////////////////////////////////////////
  void loadConfigFromFile(const std::string &cfgPath)
  {
    std::ifstream input(cfgPath);
    *this = json::parse(input);
    interpretConfig();
  }

  void loadConfigFromString(const std::string &cfgPath)
  {
    *this = json::parse(cfgPath);
    interpretConfig();
  }

  auto getPipeline() const -> Pipeline
  {
    return enumPipeline;
  }

  auto getMinColocFactor() const -> float
  {
    return min_coloc_factor;
  }

  auto getPixelInMicrometer() const -> float
  {
    return pixel_in_micrometer;
  }

  auto getWithControlImage() const -> bool
  {
    return with_control_images;
  }

  auto getWithDetailReport() const -> bool
  {
    return with_detailed_report;
  }

  auto getChannels() const -> const std::multimap<ChannelSettings::Type, ChannelSettings> &
  {
    return orderedChannels;
  }

  auto getChannels(ChannelSettings::Type type) const -> std::vector<ChannelSettings>
  {
    auto range = orderedChannels.equal_range(type);
    std::vector<ChannelSettings> returnVector;
    std::transform(range.first, range.second, std::back_inserter(returnVector),
                   [](const std::pair<ChannelSettings::Type, ChannelSettings> &element) { return element.second; });

    return returnVector;
  }

private:
  void interpretConfig()
  {
    for(auto &ch : channels) {
      ch.interpretConfig();
      // Move from vector to ordered map
      orderedChannels.emplace(ch.getType(), std::move(ch));
    }
    channels.clear();
    stringToPipeline();
  }
  //
  // Pipeline to analyze the pictures with
  // [NUCLEUS_COUNT, EV_COUNT, EV_COLOC, EV_COLOC_IN_CELLS]
  //
  std::string pipeline;
  Pipeline enumPipeline;
  void stringToPipeline();

  //
  // Settings for the image channels
  //
  std::vector<ChannelSettings> channels;
  std::multimap<ChannelSettings::Type, ChannelSettings> orderedChannels;

  //
  // Minimum of area overlapping to identify two particles as colocalize
  // Value between [0-1]
  //
  float min_coloc_factor;

  //
  // How many micrometers are represented by one pixel
  // Value in [um]
  //
  float pixel_in_micrometer;

  //
  // With or without control images
  // [true, false]
  //
  bool with_control_images;

  //
  // Detailed report on
  // [true, false]
  //
  bool with_detailed_report;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalyzeSettings, pipeline, channels, min_coloc_factor, pixel_in_micrometer,
                                 with_control_images, with_detailed_report);
};
}    // namespace joda::settings::json
