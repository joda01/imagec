///
/// \file      store_to_db.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "store_to_db.hpp"
#include "backend/helper/duration_count/duration_count.h"

namespace joda::cmd::functions {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void StoreToDb::execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
                        ObjectsListMap &result)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StoreToDb::prepareDetailReportAdding(processor::ProcessContext &context) -> DetailReportAdder
{
  auto connection = context.database->acquire();
  // connection->BeginTransaction();
  auto objects    = std::make_shared<duckdb::Appender>(*connection, "objects");
  auto imageStats = std::make_shared<duckdb::Appender>(*connection, "image_stats");
  return {connection, objects, imageStats};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void StoreToDb::writePredatedData(const DetailReportAdder &adders)
{
  auto id = DurationCount::start("Close");
  adders.imageStats->Close();
  adders.objects->Close();
  // connection->Commit();
  DurationCount::stop(id);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void StoreToDb::appendToDetailReport(const DetailReportAdder &appender,
                                     const joda::image::detect::DetectionResponse &results,
                                     const joda::settings::ChannelSettingsMeta &channelSettings,
                                     const joda::ome::TileToLoad &tileIdx, const joda::ome::OmeInfo &imgProps,
                                     const std::filesystem::path &imagePath)
{
  struct ImageStats
  {
    double sumVal = 0;
    std::multiset<double> values;

    [[nodiscard]] double sum() const
    {
      return sumVal;
    }

    [[nodiscard]] double cnt() const
    {
      return values.size();
    }

    [[nodiscard]] double min() const
    {
      if(values.empty()) {
        return 0;
      }
      return *values.begin();
    }

    [[nodiscard]] double max() const
    {
      if(values.empty()) {
        return 0;
      }
      return *values.rbegin();
    }

    [[nodiscard]] double median() const
    {
      auto it = values.begin();
      std::advance(it, values.size() / 2);
      double median = *it;
      if(values.size() % 2 != 0) {
        // If the number of elements is odd, return the middle element
        return *it;
      }    // If the number of elements is even, return the average of the two middle elements
      auto it1 = it;
      auto it2 = std::prev(it);
      return (*it1 + *it2) / 2.0;
    }
    [[nodiscard]] double avg() const
    {
      if(values.empty()) {
        return 0.0;    // Handle the case for an empty set
      }

      return sumVal / static_cast<double>(values.size());
    }

    [[nodiscard]] double stddev(double mean) const
    {
      if(values.size() <= 1) {
        return 0.0;    // Standard deviation is 0 if there are no elements or only one element
      }

      double sumOfSquares = 0.0;
      for(const auto &num : values) {
        sumOfSquares += (num - mean) * (num - mean);
      }

      double variance = sumOfSquares / static_cast<double>(values.size() - 1);    // Using n-1 for an unbiased estimator
      return std::sqrt(variance);
    }
  };
  std::map<MeasureChannelId, ImageStats> imgStats;
  int32_t tileNr = imgProps.getImageInfo().resolutions.at(0).toTileNr(tileIdx);
  try {
    auto id          = DurationCount::start("Append to detail report");
    uint64_t imageId = calcImagePathHash(mExperimentSettings.runId, imagePath);

    auto controlImagePath = createControlImage(results, channelSettings, tileNr, imgProps, imagePath);
    auto channelId        = toChannelIndex(channelSettings.channelIdx);
    mDatabase->createImageChannel(db::ImageChannelMeta{.analyzeId        = mAnalyzeId,
                                                       .imageId          = imageId,
                                                       .channelId        = channelId,
                                                       .validity         = toChannelValidity(results.responseValidity),
                                                       .invalidateAll    = results.invalidateWholeImage,
                                                       .controlImagePath = controlImagePath});

    int64_t xMul = tileIdx.tileX * tileIdx.tileWidth;
    int64_t yMul = tileIdx.tileY * tileIdx.tileHeight;

    uint64_t roiIdx = 0;
    auto id2 = DurationCount::start("loop db prepare >" + std::to_string(results.result->size()) + "<.");    // 30ms

    auto uuid = duckdb::Value::UUID(mAnalyzeId);
    for(const auto &roi : *results.result) {
      uint64_t index = roiIdx;
      roiIdx++;
      // db::Data &chan = objects[index];

      duckdb::vector<duckdb::Value> keys;
      duckdb::vector<duckdb::Value> vals;

      auto isValid    = roi.isValid();
      auto addToStats = [&imgStats, &keys, &vals, &isValid](const MeasureChannelId &ch, double val) {
        keys.emplace_back(duckdb::Value::UINTEGER((uint32_t) ch));
        vals.emplace_back(duckdb::Value::DOUBLE(val));
        if(isValid) {
          imgStats[ch].values.emplace(val);
          imgStats[ch].sumVal += val;
        }
      };

      auto addToStatsValidity = [&imgStats, &keys, &vals](const MeasureChannelId &ch, uint8_t val) {
        keys.emplace_back(duckdb::Value::UINTEGER((uint32_t) ch));
        vals.emplace_back(duckdb::Value::TINYINT(val));
        imgStats[ch].values.emplace(val);
        imgStats[ch].sumVal += val;
      };

      // Measure channels

      addToStats(MeasureChannelId(MeasureChannel::CONFIDENCE, ChannelIndex::ME), roi.getConfidence());
      addToStats(MeasureChannelId(MeasureChannel::AREA_SIZE, ChannelIndex::ME), roi.getAreaSize());
      addToStats(MeasureChannelId(MeasureChannel::PERIMETER, ChannelIndex::ME), roi.getPerimeter());
      addToStats(MeasureChannelId(MeasureChannel::CIRCULARITY, ChannelIndex::ME), roi.getCircularity());
      addToStatsValidity(MeasureChannelId(MeasureChannel::VALID, ChannelIndex::ME), roi.isValid() ? 1 : 0);
      addToStatsValidity(MeasureChannelId(MeasureChannel::INVALID, ChannelIndex::ME), roi.isValid() ? 0 : 1);
      addToStats(MeasureChannelId(MeasureChannel::CENTER_OF_MASS_X, ChannelIndex::ME),
                 static_cast<double>(roi.getCenterOfMass().x) + xMul);
      addToStats(MeasureChannelId(MeasureChannel::CENTER_OF_MASS_Y, ChannelIndex::ME),
                 static_cast<double>(roi.getCenterOfMass().y) + yMul);
      addToStats(MeasureChannelId(MeasureChannel::BOUNDING_BOX_WIDTH, ChannelIndex::ME),
                 static_cast<double>(roi.getBoundingBox().width));
      addToStats(MeasureChannelId(MeasureChannel::BOUNDING_BOX_HEIGHT, ChannelIndex::ME),
                 static_cast<double>(roi.getBoundingBox().height));

      double intensityAvg = 0;
      double intensityMin = 0;
      double intensityMax = 0;
      if(roi.getIntensity().contains(channelSettings.channelIdx)) {
        auto intensityMe = roi.getIntensity().at(channelSettings.channelIdx);
        intensityAvg     = intensityMe.intensity;
        intensityMin     = intensityMe.intensityMin;
        intensityMax     = intensityMe.intensityMax;
      }

      addToStats(MeasureChannelId(MeasureChannel::INTENSITY_AVG, ChannelIndex::ME), intensityAvg);
      addToStats(MeasureChannelId(MeasureChannel::INTENSITY_MIN, ChannelIndex::ME), intensityMin);
      addToStats(MeasureChannelId(MeasureChannel::INTENSITY_MAX, ChannelIndex::ME), intensityMax);

      //
      // Intensity channels
      //
      for(const auto &[idx, intensity] : roi.getIntensity()) {
        if(idx != channelSettings.channelIdx) {
          addToStats(MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, toChannelIndex(idx)),
                     intensity.intensity);

          addToStats(MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, toChannelIndex(idx)),
                     intensity.intensityMin);

          addToStats(MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, toChannelIndex(idx)),
                     intensity.intensityMax);
        }
      }

      //
      // Counting channels
      //
      for(const auto &[idx, intersecting] : roi.getIntersectingRois()) {
        {
          addToStats(MeasureChannelId(MeasureChannel::CROSS_CHANNEL_COUNT, toChannelIndex(idx)),
                     intersecting.roiValid.size());
        }
      }

      {
        std::lock_guard<std::mutex> lock(mAppenderMutex);
        appender.objects->BeginRow();
        appender.objects->Append(uuid);
        appender.objects->Append<uint64_t>(imageId);
        appender.objects->Append<uint16_t>(static_cast<uint16_t>(channelId));
        appender.objects->Append<uint32_t>(index);
        appender.objects->Append<uint16_t>(tileNr);
        appender.objects->Append<uint64_t>(toValidity(roi.getValidity()).to_ulong());
        auto mapToInsert = duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER),
                                              duckdb::LogicalType(duckdb::LogicalTypeId::DOUBLE), keys, vals);
        appender.objects->Append<duckdb::Value>(mapToInsert);    // 0.004ms
        appender.objects->EndRow();
      }
    }
    DurationCount::stop(id2);

    {
      struct DbStats
      {
        duckdb::vector<duckdb::Value> keys;
        duckdb::vector<duckdb::Value> vals;

        void addToStats(const MeasureChannelId &ch, double val)
        {
          keys.emplace_back(duckdb::Value::UINTEGER((uint32_t) ch));
          vals.emplace_back(duckdb::Value::DOUBLE(val));
        };
      };
      DbStats statsSum;
      DbStats statsCnt;
      DbStats statsMin;
      DbStats statsMax;
      DbStats statsMedian;
      DbStats statsAvg;
      DbStats statsStddev;

      for(const auto &[ch, stats] : imgStats) {
        statsSum.addToStats(ch, stats.sum());
        statsCnt.addToStats(ch, stats.cnt());
        statsMin.addToStats(ch, stats.min());
        statsMax.addToStats(ch, stats.max());
        statsMedian.addToStats(ch, stats.median());
        auto avg = stats.avg();
        statsAvg.addToStats(ch, avg);
        statsStddev.addToStats(ch, stats.stddev(avg));
      }

      auto addToMap = [&appender](const DbStats &stats) {
        auto mapToInsert =
            duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER),
                               duckdb::LogicalType(duckdb::LogicalTypeId::DOUBLE), stats.keys, stats.vals);
        appender.imageStats->Append<duckdb::Value>(mapToInsert);    // 0.004ms
      };

      std::lock_guard<std::mutex> lock(mAppenderMutex);
      appender.imageStats->BeginRow();
      appender.imageStats->Append(uuid);
      appender.imageStats->Append<uint64_t>(imageId);
      appender.imageStats->Append<uint16_t>(static_cast<uint16_t>(channelId));
      appender.imageStats->Append<uint16_t>(tileNr);
      addToMap(statsSum);
      addToMap(statsCnt);
      addToMap(statsMin);
      addToMap(statsMax);
      addToMap(statsMedian);
      addToMap(statsAvg);
      addToMap(statsStddev);
      appender.imageStats->EndRow();
    }

    DurationCount::stop(id);
  } catch(const std::exception &ex) {
    joda::log::logWarning("Append to detail report:" + std::string(ex.what()));
  }
}

}    // namespace joda::cmd::functions
