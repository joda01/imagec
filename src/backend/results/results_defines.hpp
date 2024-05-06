///
/// \file      results_defines.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#pragma once

#include <cstdint>
#include <string>
#include "backend/settings/channel/channel_index.hpp"
#include "results_reporting_settings.hpp"

namespace joda::settings {
class AnalyzeSettings;
}    // namespace joda::settings

namespace joda::results {

///
/// \class      MeasureChannelKey
/// \author     Joachim Danmayr
/// \brief
///
class MeasureChannelKey
{
private:
  static inline constexpr uint32_t MEASURE_CHANNEL_MASK            = 0x00FF0000;
  static inline constexpr uint32_t MEASURE_CHANNEL_STATS_01_MASK   = 0x00000F00;
  static inline constexpr uint32_t MEASURE_CHANNEL_STATS_02_MASK   = 0x0000F000;
  static inline constexpr uint32_t MEASURE_CHANNEL_STATS_BOTH_MASK = 0x0000FF00;

  static inline constexpr uint32_t MEASURE_CHANNEL_STATS_AND_MASK = 0x00FFFF00;
  static inline constexpr uint32_t MEASURE_CHANNEL_INDEX_MASK     = 0x000000FF;

public:
  /////////////////////////////////////////////////////
  MeasureChannelKey(const std::string &stringValue)
  {
    // Remove the "0x" prefix
    if(stringValue.starts_with("0x")) {
      std::string hexValue = stringValue.substr(2);
      value                = std::stoul(hexValue, nullptr, 16);
    } else {
      value = std::stoul(stringValue);
    }
  }

  MeasureChannelKey(uint32_t value) : value(value)
  {
  }

  MeasureChannelKey(ReportingSettings::MeasureChannelsCombi value) : value(static_cast<uint32_t>(value))
  {
  }

  MeasureChannelKey(const MeasureChannelKey &measureChannel, ReportingSettings::MeasureChannelStat measureChannelStat) :
      value(
          ((measureChannel.value & ~MEASURE_CHANNEL_STATS_01_MASK) | (static_cast<uint32_t>(measureChannelStat) << 8)))
  {
  }

  MeasureChannelKey(const MeasureChannelKey &measureChannel, ReportingSettings::MeasureChannelStat measureChannelStat02,
                    ReportingSettings::MeasureChannelStat measureChannelStat01) :
      value((measureChannel.value & ~MEASURE_CHANNEL_STATS_BOTH_MASK) |
            (static_cast<uint32_t>(measureChannelStat02) << 12) | (static_cast<uint32_t>(measureChannelStat01) << 8))
  {
  }

  MeasureChannelKey(ReportingSettings::MeasureChannels measureChannel,
                    ReportingSettings::MeasureChannelStat measureChannelStat) :
      value((static_cast<uint32_t>(measureChannel) << 16 | static_cast<uint32_t>(measureChannelStat) << 8) &
            MEASURE_CHANNEL_STATS_AND_MASK)
  {
  }

  MeasureChannelKey(ReportingSettings::MeasureChannels measureChannel,
                    ReportingSettings::MeasureChannelStat measureChannelStat,
                    joda::settings::ChannelIndex channelIndex) :
      value((static_cast<uint32_t>(measureChannel) << 16 | static_cast<uint32_t>(measureChannelStat) << 8 |
             (static_cast<uint32_t>(channelIndex) & MEASURE_CHANNEL_INDEX_MASK)))
  {
  }

  MeasureChannelKey(ReportingSettings::MeasureChannels measureChannel,
                    ReportingSettings::MeasureChannelStat measureChannelStat02,
                    ReportingSettings::MeasureChannelStat measureChannelStat01,
                    joda::settings::ChannelIndex channelIndex) :
      value(((static_cast<uint32_t>(measureChannel) << 16) | (static_cast<uint32_t>(measureChannelStat02) << 12) |
             (static_cast<uint32_t>(measureChannelStat01) << 8) |
             (static_cast<uint32_t>(channelIndex) & MEASURE_CHANNEL_INDEX_MASK)))
  {
  }

  MeasureChannelKey(ReportingSettings::MeasureChannels measureChannel, joda::settings::ChannelIndex channelIndex) :
      value((static_cast<uint32_t>(measureChannel) << 16 |
             static_cast<uint32_t>(ReportingSettings::MeasureChannelStat::VAL) << 8 |
             (static_cast<uint32_t>(channelIndex) & MEASURE_CHANNEL_INDEX_MASK)))
  {
  }

  bool operator<(const MeasureChannelKey &other) const
  {
    return value < other.value;
  }

  [[nodiscard]] ReportingSettings::MeasureChannels getMeasureChannel() const;
  [[nodiscard]] ReportingSettings::MeasureChannelStat getMeasureStats01() const;
  [[nodiscard]] ReportingSettings::MeasureChannelStat getMeasureStats02() const;

  [[nodiscard]] ReportingSettings::MeasureChannelsCombi getMeasreChannelAndStatsCombo() const;
  [[nodiscard]] joda::settings::ChannelIndex getChannelIndex() const;
  [[nodiscard]] std::string measurementStatsToString() const;
  [[nodiscard]] std::string measurementChannelsToString() const;
  [[nodiscard]] std::string measurementChannelsToString(const joda::settings::AnalyzeSettings &settings) const;
  [[nodiscard]] auto toString() const
  {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << value;
    return ss.str();
  }

private:
  /////////////////////////////////////////////////////
  uint32_t value;
};

}    // namespace joda::results
