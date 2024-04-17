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

namespace joda::settings {

class ChannelReportingSettings
{
public:
  enum class MeasureChannels : int
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
    std::set<MeasureChannels> measureChannels{
        MeasureChannels::CONFIDENCE,
        MeasureChannels::AREA_SIZE,
        MeasureChannels::CIRCULARITY,
        MeasureChannels::VALIDITY,
        MeasureChannels::INVALIDITY,
        MeasureChannels::INTENSITY_AVG,
        MeasureChannels::INTENSITY_MIN,
        MeasureChannels::INTENSITY_MAX,
        MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL,
        MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL,
        MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL,
        MeasureChannels::COUNT_CROSS_CHANNEL,
    };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(OverviewReport, measureChannels);
  };

  class Heatmap
  {
  public:
    std::set<MeasureChannels> measureChannels{
        MeasureChannels::VALIDITY,
        MeasureChannels::INTENSITY_AVG,
        MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL,
        MeasureChannels::COUNT_CROSS_CHANNEL,
    };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Heatmap, measureChannels);
  };

  DetailReport detail;
  OverviewReport overview;
  Heatmap heatmap;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelReportingSettings, detail, overview, heatmap);
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    ChannelReportingSettings::MeasureChannels,
    {
        {ChannelReportingSettings::MeasureChannels::CONFIDENCE, "Confidence"},
        {ChannelReportingSettings::MeasureChannels::AREA_SIZE, "AreaSize"},
        {ChannelReportingSettings::MeasureChannels::CIRCULARITY, "Circularity"},
        {ChannelReportingSettings::MeasureChannels::VALIDITY, "Validity"},
        {ChannelReportingSettings::MeasureChannels::INVALIDITY, "Invalidity"},
        {ChannelReportingSettings::MeasureChannels::CENTER_OF_MASS_X, "CenterOfMassX"},
        {ChannelReportingSettings::MeasureChannels::CENTER_OF_MASS_Y, "CenterOfMassY"},
        {ChannelReportingSettings::MeasureChannels::INTENSITY_AVG, "IntensityAvg"},
        {ChannelReportingSettings::MeasureChannels::INTENSITY_MIN, "IntensityMin"},
        {ChannelReportingSettings::MeasureChannels::INTENSITY_MAX, "IntensityMax"},
        {ChannelReportingSettings::MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL, "CrossChannelIntensityAvg"},
        {ChannelReportingSettings::MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL, "CrossChannelIntensityMin"},
        {ChannelReportingSettings::MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL, "CrossChannelIntensityMax"},
        {ChannelReportingSettings::MeasureChannels::COUNT_CROSS_CHANNEL, "CrossChannelCount"},

    })

}    // namespace joda::settings
