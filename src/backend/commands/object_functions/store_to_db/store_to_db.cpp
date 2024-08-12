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
void StoreToDb::appendToDetailReport(const DetailReportAdder &appender, processor::ProcessContext &context,
                                     ObjectsListMap &result)
{
  try {
    auto id          = DurationCount::start("Append to detail report");
    uint64_t imageId = calcImagePathHash(context.imagePath);
    std::string controlImagePath;
    context.database->createImageChannel(db::ImageChannelMeta{.analyzeId = mAnalyzeId,
                                                              .imageId   = imageId,
                                                              .channelId = context.cStack,
                                                              .validity  = toChannelValidity(results.responseValidity),
                                                              .invalidateAll    = results.invalidateWholeImage,
                                                              .controlImagePath = controlImagePath});

    uint64_t roiIdx = 0;
    auto id2        = DurationCount::start("loop db prepare >" + std::to_string(result.size()) + "<.");    // 30ms

    for(const auto &roi : result) {
      uint64_t index = roiIdx;
      roiIdx++;
      // db::Data &chan = objects[index];

      duckdb::vector<duckdb::Value> keys;
      duckdb::vector<duckdb::Value> vals;

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

  } catch(const std::exception &ex) {
    joda::log::logWarning("Append to detail report:" + std::string(ex.what()));
  }
}

}    // namespace joda::cmd::functions
