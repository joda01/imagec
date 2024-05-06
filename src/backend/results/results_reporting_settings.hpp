///
/// \file      reporting_settings.hpp
/// \author
/// \date      2024-04-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::results {

class ReportingSettings
{
public:
  enum class MeasureChannelStat
  {
    VAL     = 0x0,
    AVG     = 0x1,
    SUM     = 0x2,
    MIN     = 0x3,
    MAX     = 0x4,
    CNT     = 0x5,
    STD_DEV = 0x6

  };

  enum class MeasureChannels : uint32_t
  {
    CONFIDENCE                  = 0x0001,
    AREA_SIZE                   = 0x0002,
    PERIMETER                   = 0x0003,
    CIRCULARITY                 = 0x0004,
    VALIDITY                    = 0x0005,
    INVALIDITY                  = 0x0006,
    CENTER_OF_MASS_X            = 0x0007,
    CENTER_OF_MASS_Y            = 0x0008,
    INTENSITY_AVG               = 0x0009,
    INTENSITY_MIN               = 0x000A,
    INTENSITY_MAX               = 0x000B,
    INTENSITY_AVG_CROSS_CHANNEL = 0x000C,
    INTENSITY_MIN_CROSS_CHANNEL = 0x000D,
    INTENSITY_MAX_CROSS_CHANNEL = 0x000E,
    COUNT_CROSS_CHANNEL         = 0x000F,
  };

  enum class MeasureChannelsCombi : uint32_t
  {
    CONFIDENCE_AVG = (static_cast<uint32_t>(MeasureChannels::CONFIDENCE) << 16) |
                     (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                     (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    AREA_SIZE_AVG = (static_cast<uint32_t>(MeasureChannels::AREA_SIZE) << 16) |
                    (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                    (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    PERIMETER_AVG = (static_cast<uint32_t>(MeasureChannels::PERIMETER) << 16) |
                    (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                    (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    CIRCULARITY_AVG = (static_cast<uint32_t>(MeasureChannels::CIRCULARITY) << 16) |
                      (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                      (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    VALIDITY_AVG = (static_cast<uint32_t>(MeasureChannels::VALIDITY) << 16) |
                   (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                   (static_cast<uint32_t>(MeasureChannelStat::SUM) << 8),
    INVALIDITY_AVG = (static_cast<uint32_t>(MeasureChannels::INVALIDITY) << 16) |
                     (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                     (static_cast<uint32_t>(MeasureChannelStat::SUM) << 8),
    CENTER_OF_MASS_X_AVG = (static_cast<uint32_t>(MeasureChannels::CENTER_OF_MASS_X) << 16) |
                           (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                           (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    CENTER_OF_MASS_Y_AVG = (static_cast<uint32_t>(MeasureChannels::CENTER_OF_MASS_Y) << 16) |
                           (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                           (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    INTENSITY_AVG_AVG = (static_cast<uint32_t>(MeasureChannels::INTENSITY_AVG) << 16) |
                        (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                        (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    INTENSITY_MIN_AVG = (static_cast<uint32_t>(MeasureChannels::INTENSITY_MIN) << 16) |
                        (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                        (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    INTENSITY_MAX_AVG = (static_cast<uint32_t>(MeasureChannels::INTENSITY_MAX) << 16) |
                        (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                        (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    INTENSITY_AVG_CROSS_CHANNEL_AVG = (static_cast<uint32_t>(MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL) << 16) |
                                      (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                                      (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    INTENSITY_MIN_CROSS_CHANNEL_AVG = (static_cast<uint32_t>(MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL) << 16) |
                                      (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                                      (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    INTENSITY_MAX_CROSS_CHANNEL_AVG = (static_cast<uint32_t>(MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL) << 16) |
                                      (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                                      (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    COUNT_CROSS_CHANNEL_AVG = (static_cast<uint32_t>(MeasureChannels::COUNT_CROSS_CHANNEL) << 16) |
                              (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                              (static_cast<uint32_t>(MeasureChannelStat::AVG) << 8),
    COUNT_CROSS_CHANNEL_SUM = (static_cast<uint32_t>(MeasureChannels::COUNT_CROSS_CHANNEL) << 16) |
                              (static_cast<uint32_t>(MeasureChannelStat::VAL) << 12) |
                              (static_cast<uint32_t>(MeasureChannelStat::SUM) << 8),
  };

  class DetailReport
  {
  public:
    std::set<MeasureChannels> measureChannels{MeasureChannels::CONFIDENCE,
                                              MeasureChannels::AREA_SIZE,
                                              MeasureChannels::PERIMETER,
                                              MeasureChannels::CIRCULARITY,
                                              MeasureChannels::VALIDITY,
                                              MeasureChannels::INVALIDITY,
                                              MeasureChannels::CENTER_OF_MASS_X,
                                              MeasureChannels::CENTER_OF_MASS_Y,
                                              MeasureChannels::INTENSITY_AVG,
                                              MeasureChannels::INTENSITY_MIN,
                                              MeasureChannels::INTENSITY_MAX,
                                              MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL,
                                              MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL,
                                              MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL,
                                              MeasureChannels::COUNT_CROSS_CHANNEL};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(DetailReport, measureChannels);
  };
  class OverviewReport
  {
  public:
    std::set<MeasureChannelsCombi> measureChannels{
        MeasureChannelsCombi::CONFIDENCE_AVG,
        MeasureChannelsCombi::AREA_SIZE_AVG,
        MeasureChannelsCombi::CIRCULARITY_AVG,
        MeasureChannelsCombi::VALIDITY_AVG,
        MeasureChannelsCombi::INVALIDITY_AVG,
        MeasureChannelsCombi::INTENSITY_AVG_AVG,
        MeasureChannelsCombi::INTENSITY_MIN_AVG,
        MeasureChannelsCombi::INTENSITY_MAX_AVG,
        MeasureChannelsCombi::INTENSITY_AVG_CROSS_CHANNEL_AVG,
        MeasureChannelsCombi::INTENSITY_MIN_CROSS_CHANNEL_AVG,
        MeasureChannelsCombi::INTENSITY_MAX_CROSS_CHANNEL_AVG,
        MeasureChannelsCombi::COUNT_CROSS_CHANNEL_AVG,
        MeasureChannelsCombi::COUNT_CROSS_CHANNEL_SUM,
    };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(OverviewReport, measureChannels);
  };

  DetailReport detail;
  OverviewReport overview;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ReportingSettings, detail, overview);
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    ReportingSettings::MeasureChannelsCombi,
    {
        {ReportingSettings::MeasureChannelsCombi::CONFIDENCE_AVG, "ConfidenceAvg"},
        {ReportingSettings::MeasureChannelsCombi::AREA_SIZE_AVG, "AreaSizeAvg"},
        {ReportingSettings::MeasureChannelsCombi::CIRCULARITY_AVG, "CircularityAvf"},
        {ReportingSettings::MeasureChannelsCombi::VALIDITY_AVG, "ValidityAvf"},
        {ReportingSettings::MeasureChannelsCombi::INVALIDITY_AVG, "InvalidityAvf"},
        {ReportingSettings::MeasureChannelsCombi::CENTER_OF_MASS_X_AVG, "CenterOfMassXAvg"},
        {ReportingSettings::MeasureChannelsCombi::CENTER_OF_MASS_Y_AVG, "CenterOfMassYAvg"},
        {ReportingSettings::MeasureChannelsCombi::INTENSITY_AVG_AVG, "IntensityAvgAvg"},
        {ReportingSettings::MeasureChannelsCombi::INTENSITY_MIN_AVG, "IntensityMinAvg"},
        {ReportingSettings::MeasureChannelsCombi::INTENSITY_MAX_AVG, "IntensityMaxAvg"},
        {ReportingSettings::MeasureChannelsCombi::INTENSITY_AVG_CROSS_CHANNEL_AVG, "CrossChannelIntensityAvgAvg"},
        {ReportingSettings::MeasureChannelsCombi::INTENSITY_MIN_CROSS_CHANNEL_AVG, "CrossChannelIntensityMinAvg"},
        {ReportingSettings::MeasureChannelsCombi::INTENSITY_MAX_CROSS_CHANNEL_AVG, "CrossChannelIntensityMaxAvg"},
        {ReportingSettings::MeasureChannelsCombi::COUNT_CROSS_CHANNEL_AVG, "CrossChannelCountAvg"},
        {ReportingSettings::MeasureChannelsCombi::COUNT_CROSS_CHANNEL_SUM, "CrossChannelCountSum"},
    })

NLOHMANN_JSON_SERIALIZE_ENUM(
    ReportingSettings::MeasureChannels,
    {
        {ReportingSettings::MeasureChannels::CONFIDENCE, "Confidence"},
        {ReportingSettings::MeasureChannels::AREA_SIZE, "AreaSize"},
        {ReportingSettings::MeasureChannels::CIRCULARITY, "Circularity"},
        {ReportingSettings::MeasureChannels::VALIDITY, "Validity"},
        {ReportingSettings::MeasureChannels::INVALIDITY, "Invalidity"},
        {ReportingSettings::MeasureChannels::CENTER_OF_MASS_X, "CenterOfMassX"},
        {ReportingSettings::MeasureChannels::CENTER_OF_MASS_Y, "CenterOfMassY"},
        {ReportingSettings::MeasureChannels::INTENSITY_AVG, "IntensityAvg"},
        {ReportingSettings::MeasureChannels::INTENSITY_MIN, "IntensityMin"},
        {ReportingSettings::MeasureChannels::INTENSITY_MAX, "IntensityMax"},
        {ReportingSettings::MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL, "CrossChannelIntensityAvg"},
        {ReportingSettings::MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL, "CrossChannelIntensityMin"},
        {ReportingSettings::MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL, "CrossChannelIntensityMax"},
        {ReportingSettings::MeasureChannels::COUNT_CROSS_CHANNEL, "CrossChannelCount"},
    })

}    // namespace joda::results
