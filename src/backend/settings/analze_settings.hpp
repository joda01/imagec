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
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include "backend/settings/reporting_settings.hpp"
#include <catch2/catch_config.hpp>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
#include "channel_settings.hpp"
#include "pipeline_settings.hpp"

namespace joda::settings::json {

using json = nlohmann::json;

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
    std::ostringstream oss;
    oss << input.rdbuf();    // Read the file buffer into the ostringstream
    std::string read = oss.str();
    *this            = json::parse(read);
    interpretConfig();

    originalJson = json::parse(read).dump(2);
    input.close();
  }

  void loadConfigFromString(const std::string &jsonString)
  {
    *this = json::parse(jsonString);
    interpretConfig();
    originalJson = json::parse(jsonString).dump(2);
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

  [[nodiscard]] auto getChannelsVector() const -> const std::vector<ChannelSettings> &
  {
    return channels;
  }

  [[nodiscard]] auto getChannelsOrderedByChannelIndex() const -> const std::map<int, ChannelSettings> &
  {
    return orderedChannelsByChannelIndex;
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

  [[nodiscard]] auto getChannelByChannelIndex(uint32_t idx) const -> ChannelSettings
  {
    if(!orderedChannelsByChannelIndex.contains(idx)) {
      throw std::runtime_error("getChannelByChannelIndex: Channel with index >" + std::to_string(idx) +
                               "< does not exist.");
    }
    return orderedChannelsByChannelIndex.at(idx);
  }

  [[nodiscard]] auto getChannelNameOfIndex(uint32_t idx) const -> std::string
  {
    std::string suffix;
    // Special channels
    if(idx >= PipelineStepSettings::INTERSECTION_INDEX_OFFSET) {
      idx    = idx - PipelineStepSettings::INTERSECTION_INDEX_OFFSET;
      suffix = " intersection";
    } else if(idx >= 100) {
      if(!orderedPipelinesByChannelIndex.contains(idx)) {
        throw std::runtime_error("getChannelByChannelIndex: Channel with index >" + std::to_string(idx) +
                                 "< does not exist.");
      }
      return orderedPipelinesByChannelIndex.at(idx).getName();
    }
    return getChannelByChannelIndex(idx).getChannelInfo().getName() + suffix;
  }

  [[nodiscard]] auto getNumberOfCrossChannelIntensityMeasurementForChannel(uint32_t idx) const -> std::set<int32_t>
  {
    // Special channels
    if(idx >= PipelineStepSettings::INTERSECTION_INDEX_OFFSET) {
      idx = idx - PipelineStepSettings::INTERSECTION_INDEX_OFFSET;
      return {};
    } else if(idx >= 100) {
      if(!orderedPipelinesByChannelIndex.contains(idx)) {
        throw std::runtime_error("getNumberOfCrossChannelIntensityMeasurementForChannel: Channel with index >" +
                                 std::to_string(idx) + "< does not exist.");
      }
      if(orderedPipelinesByChannelIndex.at(idx).getVoronoi() != nullptr) {
        return orderedPipelinesByChannelIndex.at(idx).getVoronoi()->getCrossChannelIntensityIndexes();
      } else {
        return {};
      }
    }
    return getChannelByChannelIndex(idx).getCrossChannelSettings().getCrossChannelIntensityIndexes();
  }

  [[nodiscard]] auto getNumberOfCrossChannelCountMeasurementForChannel(uint32_t idx) const -> std::set<int32_t>
  {
    // Special channels
    if(idx >= PipelineStepSettings::INTERSECTION_INDEX_OFFSET) {
      idx = idx - PipelineStepSettings::INTERSECTION_INDEX_OFFSET;
      return {};
    } else if(idx >= 100) {
      if(!orderedPipelinesByChannelIndex.contains(idx)) {
        throw std::runtime_error("getNumberOfCrossChannelCountMeasurementForChannel: Channel with index >" +
                                 std::to_string(idx) + "< does not exist.");
      }
      if(orderedPipelinesByChannelIndex.at(idx).getVoronoi() != nullptr) {
        return orderedPipelinesByChannelIndex.at(idx).getVoronoi()->getCrossChannelCountIndexes();
      } else {
        return {};
      }
    }
    return getChannelByChannelIndex(idx).getCrossChannelSettings().getCrossChannelCountIndexes();
  }

  [[nodiscard]] auto getReportingSettingsForChannel(uint32_t idx) const -> const ReportingSettings
  {
    // Special channels
    if(idx >= PipelineStepSettings::INTERSECTION_INDEX_OFFSET) {
      idx = idx - PipelineStepSettings::INTERSECTION_INDEX_OFFSET;
      return {};
    } else if(idx >= 100) {
      if(!orderedPipelinesByChannelIndex.contains(idx)) {
        throw std::runtime_error("getReportingSettingsForChannel: Channel with index >" + std::to_string(idx) +
                                 "< does not exist.");
      }
      if(orderedPipelinesByChannelIndex.at(idx).getVoronoi() != nullptr) {
        return orderedPipelinesByChannelIndex.at(idx).getVoronoi()->getReportingSettings();
      } else {
        return {};
      }
    }
    return getChannelByChannelIndex(idx).getReportingSettings();
  }

  [[nodiscard]] auto getOptions() const -> const AnalyzeSettingsOptions &
  {
    return options;
  }

  [[nodiscard]] auto getReportingSettings() const -> const AnalyzeSettingsReporting &
  {
    return reporting;
  }

  [[nodiscard]] auto getPipelineSteps() const -> const std::vector<PipelineStepSettings> &
  {
    return pipeline_steps;
  }

private:
  void interpretConfig()
  {
    for(int idx = 0; idx < channels.size(); idx++) {
      auto &ch = channels[idx];
      ch.interpretConfig(idx);
      // Move from vector to ordered map
      orderedChannels.emplace(ch.getChannelInfo().getType(), ch);
      orderedChannelsByChannelIndex.emplace(ch.getChannelInfo().getChannelIndex(), ch);
    }

    for(auto &step : pipeline_steps) {
      step.interpretConfig();
      int idx = step.getChannelIndex();
      if(idx >= 0) {
        orderedPipelinesByChannelIndex.emplace(idx, step);
      }
    }

    reporting.interpretConfig();
  }

  //
  // Settings for the image channels
  //
  std::vector<ChannelSettings> channels;
  std::multimap<ChannelInfo::Type, ChannelSettings> orderedChannels;
  std::map<int, ChannelSettings> orderedChannelsByChannelIndex;

  //
  // Analyses settings options
  //
  AnalyzeSettingsOptions options;

  //
  // Analyses settings options
  //
  std::vector<PipelineStepSettings> pipeline_steps;
  std::map<int, PipelineStepSettings> orderedPipelinesByChannelIndex;

  //
  // Analyses settings reporting
  //
  AnalyzeSettingsReporting reporting;

  //
  // Original unparsed json
  //
  std::string originalJson;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalyzeSettings, channels, options, pipeline_steps, reporting);
};
}    // namespace joda::settings::json
