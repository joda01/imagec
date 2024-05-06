///
/// \file      results_math.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include <stdexcept>
#include <variant>
#include "backend/results/results.hpp"
#include "backend/results/results_defines.hpp"
#include "results_reporting_settings.hpp"

namespace joda::results {

using MeasureStat = ReportingSettings::MeasureChannelStat;

struct Stats
{
  std::map<MeasureChannelKey, double> measurements;    // The measurement channels of this object

  double &operator[](const MeasureChannelKey &key)
  {
    return measurements[key];
  }

  double &emplaceWithDefault(const MeasureChannelKey &key, double defaultVal)
  {
    if(!measurements.contains(key)) {
      measurements[key] = defaultVal;
    }
    return measurements[key];
  }
};

inline Stats calcStats(const Channel &channel)
{
  Stats retStats;

  //
  // Calc stats
  //
  for(const auto &[objectKey, obj] : channel.getObjects()) {
    for(const auto &[measKey, val] : obj.getMeasurements()) {
      double &avg =
          retStats.emplaceWithDefault(MeasureChannelKey{measKey, measKey.getMeasureStats01(), MeasureStat::AVG}, 0);
      double &sum =
          retStats.emplaceWithDefault(MeasureChannelKey{measKey, measKey.getMeasureStats01(), MeasureStat::SUM}, 0);
      double &min =
          retStats.emplaceWithDefault(MeasureChannelKey{measKey, measKey.getMeasureStats01(), MeasureStat::MIN},
                                      std::numeric_limits<double>::max());
      double &max =
          retStats.emplaceWithDefault(MeasureChannelKey{measKey, measKey.getMeasureStats01(), MeasureStat::MAX},
                                      std::numeric_limits<double>::min());
      double &cnt =
          retStats.emplaceWithDefault(MeasureChannelKey{measKey, measKey.getMeasureStats01(), MeasureStat::CNT}, 0);
      double &stdDev =
          retStats.emplaceWithDefault(MeasureChannelKey{measKey, measKey.getMeasureStats01(), MeasureStat::STD_DEV}, 0);

      if(std::holds_alternative<func::ParticleValidity>(val.getVal())) {
        auto validity = std::get<func::ParticleValidity>(val.getVal());
        if(measKey.getMeasureChannel() == ReportingSettings::MeasureChannels::VALIDITY) {
          if(obj.getObjectMeta().valid) {
            sum++;
            avg    = 0;
            min    = 0;
            max    = 1;
            stdDev = 0;
          }
          cnt++;

        } else if(measKey.getMeasureChannel() == ReportingSettings::MeasureChannels::INVALIDITY) {
          if(!obj.getObjectMeta().valid) {
            sum++;
            avg    = 0;
            min    = 0;
            max    = 1;
            stdDev = 0;
          }
          cnt++;
        }
      } else if(std::holds_alternative<double>(val.getVal())) {
        if(obj.getObjectMeta().valid) {
          double values = std::get<double>(val.getVal());
          sum += values;
          min = std::min(min, values);
          max = std::max(max, values);
          cnt++;
        }
      } else {
        throw std::runtime_error("Unexpected variant found!");
      }
    }
  }

  //
  // Calc mean
  //
  for(auto &[measureKey, measure] : retStats.measurements) {
    if(measureKey.getMeasureStats01() == MeasureStat::AVG) {
      auto cnt = retStats[MeasureChannelKey{measureKey.getMeasureChannel(), measureKey.getMeasureStats02(),
                                            MeasureStat::CNT, measureKey.getChannelIndex()}];
      auto sum = retStats[MeasureChannelKey{measureKey.getMeasureChannel(), measureKey.getMeasureStats02(),
                                            MeasureStat::SUM, measureKey.getChannelIndex()}];
      if(cnt > 0) {
        measure = sum / cnt;
      }
    }
  }

  //
  // Calc variance
  //
  for(const auto &[objectKey, obj] : channel.getObjects()) {
    for(const auto &[measureKey, act] : obj.getMeasurements()) {
      if(std::holds_alternative<double>(act.getVal())) {
        if(obj.getObjectMeta().valid) {
          auto &stdDev = retStats[MeasureChannelKey{measureKey.getMeasureChannel(), measureKey.getMeasureStats02(),
                                                    MeasureStat::STD_DEV, measureKey.getChannelIndex()}];

          auto &avg   = retStats[MeasureChannelKey{measureKey.getMeasureChannel(), measureKey.getMeasureStats02(),
                                                 MeasureStat::AVG, measureKey.getChannelIndex()}];
          double diff = std::get<double>(act.getVal()) - avg;
          stdDev += diff * diff;
        }
      }
    }
  }

  //
  // Calc standard deviation
  //
  for(auto &[measureKey, measure] : retStats.measurements) {
    if(measureKey.getMeasureStats01() == MeasureStat::STD_DEV) {
      auto cnt     = retStats[MeasureChannelKey{measureKey.getMeasureChannel(), measureKey.getMeasureStats02(),
                                            MeasureStat::CNT, measureKey.getChannelIndex()}];
      auto &stdDev = retStats[MeasureChannelKey{measureKey.getMeasureChannel(), measureKey.getMeasureStats02(),
                                                MeasureStat::STD_DEV, measureKey.getChannelIndex()}];
      if(cnt > 0) {
        stdDev = std::sqrt(stdDev / (cnt - 1));
      }
    }
  }

  return retStats;
}

}    // namespace joda::results
