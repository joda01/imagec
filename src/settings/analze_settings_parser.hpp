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
#include "settings/pipeline_settings.hpp"
#include <catch2/catch_config.hpp>
#include <nlohmann/json.hpp>
#include "channel_settings.hpp"

namespace joda::settings::json {

using json = nlohmann::json;

class AnalyzeSettingsOptions final
{
public:
  [[nodiscard]] auto getPixelInMicrometer() const -> float
  {
    return pixel_in_micrometer;
  }

  [[nodiscard]] auto getWithControlImage() const -> bool
  {
    return with_control_images;
  }

  [[nodiscard]] auto getWithDetailReport() const -> bool
  {
    return with_detailed_report;
  }

private:
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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalyzeSettingsOptions, pixel_in_micrometer, with_control_images,
                                 with_detailed_report);
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

    originalJson = json::parse(input).dump();
  }

  void loadConfigFromString(const std::string &jsonString)
  {
    *this = json::parse(jsonString);
    interpretConfig();
    originalJson = json::parse(jsonString).dump();
  }

  void storeConfigToFile(const std::string &cfgPath)
  {
    std::ofstream out(cfgPath);
    out << originalJson;
    out.close();
  }

  [[nodiscard]] auto getChannels() const -> const std::multimap<ChannelInfo::Type, ChannelSettings> &
  {
    return orderedChannels;
  }

  [[nodiscard]] auto getChannels(ChannelInfo::Type type) const -> std::vector<ChannelSettings>
  {
    auto range = orderedChannels.equal_range(type);
    std::vector<ChannelSettings> returnVector;
    std::transform(range.first, range.second, std::back_inserter(returnVector),
                   [](const std::pair<ChannelInfo::Type, ChannelSettings> &element) { return element.second; });

    return returnVector;
  }

  [[nodiscard]] auto getChannelByArrayIndex(uint32_t idx) const -> ChannelSettings
  {
    return channels.at(idx);
  }

  [[nodiscard]] auto getOptions() const -> const AnalyzeSettingsOptions &
  {
    return options;
  }

private:
  void interpretConfig()
  {
    for(auto &ch : channels) {
      ch.interpretConfig();
      // Move from vector to ordered map
      orderedChannels.emplace(ch.getChannelInfo().getType(), ch);
    }
    // pipeline.interpretConfig();
  }

  //
  // Settings for the image channels
  //
  std::vector<ChannelSettings> channels;
  std::multimap<ChannelInfo::Type, ChannelSettings> orderedChannels;

  //
  // Analyses settings options
  //
  AnalyzeSettingsOptions options;

  //
  // Analyses settings options
  //
  // PipelineSettings pipeline;

  //
  // Original unparsed json
  //
  std::string originalJson;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalyzeSettings, channels, options /*, pipeline*/);
};
}    // namespace joda::settings::json
