///
/// \file      stats_for_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "stats_for_image.hpp"
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/plugins/helper.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toTable(const QueryFilter &filter) -> std::vector<joda::table::Table>
{
  std::vector<joda::table::Table> tables;
  tables.reserve(filter.clustersToExport.size());

  for(const auto &channels : filter.clustersToExport) {
    table::Table results;
    results.setTitle(channels.second.clusterName);

    auto [sql, params] = toSqlTable(filter.filter, channels);
    auto result        = filter.analyzer->select(sql, params);

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    results.setColHeader(createHeader(channels.second));

    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      results.getMutableRowHeader()[n] = std::to_string(n);
      try {
        for(int colIdx = 0; colIdx <= results.getColHeaderSize(); colIdx++) {
          results.setData(n, colIdx, table::TableCell{materializedResult->GetValue(colIdx, n).GetValue<double>(), 0, true, ""});
        }

      } catch(const duckdb::InternalException &) {
      }
    }
    tables.emplace_back(results);
  }
  return tables;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toSqlTable(const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter)
    -> std::pair<std::string, DbArgs_t>
{
  auto buildStats = [](const QueryFilter::Channel &channel) {
    std::string channels;
    for(const auto &[measurment, stats] : channel.measureChannels) {
      for(const auto stat : stats) {
        if(getType(measurment) == MeasureType::INTENSITY) {
          for(const auto [cStack, _] : channel.crossChannelStacksC) {
            channels += getStatsString(stat) + "(DISTINCT CASE WHEN t2.meas_stack_c = " + std::to_string(cStack) + " THEN " +
                        getMeasurement(measurment) + " END) AS " + getMeasurement(measurment) + "_" + std::to_string(cStack) + ",\n";
          }
        } else {
          channels += getStatsString(stat) + "(DISTINCT " + getMeasurement(measurment) + ") as getMeasurement(measurment),\n";
        }
      }
    }
    return channels;
  };

  std::string sql = "SELECT " + buildStats(channelFilter.second) +
                    ",ANY_VALUE(t1.meas_center_x) as meas_center_x,"
                    "ANY_VALUE(t1.meas_center_y) as meas_center_y"
                    "FROM\n"
                    "  objects t1\n"
                    "JOIN object_measurements t2 ON\n"
                    "  t1.object_id = t2.object_id\n"
                    "GROUP BY t1.object_id\n"
                    "ORDER BY t1.object_id\n"
                    "WHERE\n"
                    " objects.image_id=$1 AND objects.cluster_id=$2 AND objects.class_id=$3 ";
  return {sql, {filter.imageId, (uint16_t) channelFilter.first.clusterId, (uint16_t) channelFilter.first.classId}};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toHeatmap(const QueryFilter &filter) -> std::vector<joda::table::Table>
{
  std::vector<joda::table::Table> tables;
  tables.reserve(filter.clustersToExport.size());

  for(const auto &channels : filter.clustersToExport) {
    auto [result, imageInfo] = densityMap(filter.analyzer, filter.filter, channels);

    int colIdx  = 0;
    auto header = createHeader(channels.second);

    auto generateHeatmap = [&tables, &header, &filter, &result = result, imageInfo = imageInfo, &colIdx]() {
      table::Table results;

      results.setTitle(header[colIdx]);

      for(uint64_t row = 0; row < imageInfo.height; row++) {
        results.getMutableRowHeader()[row] = std::to_string(row + 1);
        for(uint64_t col = 0; col < imageInfo.width; col++) {
          results.getMutableColHeader()[col] = std::to_string(col + 1);
          results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, imageInfo.controlImgPath});
        }
      }

      auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
      auto colX               = materializedResult->ColumnCount() - 2;
      auto colY               = materializedResult->ColumnCount() - 1;

      for(size_t n = 0; n < materializedResult->RowCount(); n++) {
        try {
          uint32_t rectangleX = materializedResult->GetValue(colX, n).GetValue<double>();
          uint32_t rectangleY = materializedResult->GetValue(colY, n).GetValue<double>();
          double value        = materializedResult->GetValue(colIdx, n).GetValue<double>();

          uint32_t x = rectangleX / filter.filter.heatmapAreaSize;
          uint32_t y = rectangleY / filter.filter.heatmapAreaSize;

          std::string linkToImage = imageInfo.controlImgPath;
          results.setData(y, x, table::TableCell{value, 0, true, linkToImage});
        } catch(const duckdb::InternalException &ex) {
          std::cout << "EX " << ex.what() << std::endl;
        }
      }
      colIdx++;
      tables.emplace_back(results);
    };

    for(const auto &[measurment, stats] : channels.second.measureChannels) {
      for(const auto stat : stats) {
        if(getType(measurment) == MeasureType::INTENSITY) {
          for(const auto [cStack, _] : channels.second.crossChannelStacksC) {
            generateHeatmap();
          }
        } else {
          generateHeatmap();
        }
      }
    }
  }
  return tables;
}

///
/// \brief          Returns the heatmap in list form
/// \author         Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toHeatmapList(const QueryFilter &filter) -> std::vector<joda::table::Table>
{
  std::vector<joda::table::Table> tables;
  tables.reserve(filter.clustersToExport.size());

  for(const auto &channels : filter.clustersToExport) {
    table::Table results;

    auto [result, imageInfo] = densityMap(filter.analyzer, filter.filter, channels);

    std::map<uint32_t, std::pair<std::vector<double>, std::string>> sortedData;

    results.setColHeader(createHeader(channels.second));

    int tableRowCount = 0;
    for(uint16_t row = 0; row < imageInfo.height; row++) {
      for(uint16_t col = 0; col < imageInfo.width; col++) {
        uint32_t key                                 = ((row << 16) & 0xFFFF0000) | (col & 0xFFFF);
        char letter                                  = 'A' + row;
        results.getMutableRowHeader()[tableRowCount] = std::string(1, letter) + "" + std::to_string(col + 1);
        results.setData(tableRowCount, 0, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, imageInfo.controlImgPath});
        sortedData.emplace(key, std::pair<double, std::string>{std::numeric_limits<double>::quiet_NaN(), ""});
        tableRowCount++;
      }
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    auto colX               = materializedResult->ColumnCount() - 2;
    auto colY               = materializedResult->ColumnCount() - 1;
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      try {
        uint32_t rectangleX = materializedResult->GetValue(colX, n).GetValue<double>();
        uint32_t rectangleY = materializedResult->GetValue(colY, n).GetValue<double>();
        uint32_t x          = rectangleX / filter.filter.heatmapAreaSize;
        uint32_t y          = rectangleY / filter.filter.heatmapAreaSize;
        uint32_t key        = ((y << 16) & 0xFFFF0000) | (x & 0xFFFF);

        std::string linkToImage = imageInfo.controlImgPath;

        std::vector<double> values;
        for(int colIdx = 0; colIdx <= results.getColHeaderSize(); colIdx++) {
          double value = materializedResult->GetValue(colIdx, n).GetValue<double>();
          values.emplace_back(value);
        }
        sortedData[key] = {values, linkToImage};
      } catch(const duckdb::InternalException &ex) {
        std::cout << "EX " << ex.what() << std::endl;
      }
    }

    tableRowCount = 0;
    for(const auto &[key, data] : sortedData) {
      uint16_t row                                 = (key >> 16) & 0xFFFF;
      uint16_t col                                 = (key >> 0) & 0xFFFF;
      char letter                                  = 'A' + row;
      results.getMutableRowHeader()[tableRowCount] = std::string(1, letter) + "" + std::to_string(col + 1);
      int colIdx                                   = 0;
      for(const auto value : data.second) {
        results.setData(tableRowCount, colIdx, table::TableCell{std::get<0>(data)[colIdx], 0, true, std::get<1>(data)});
        colIdx++;
      }
      tableRowCount++;
    }
  }
  return tables;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::densityMap(db::Database *analyzer, const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter)
    -> std::tuple<std::unique_ptr<duckdb::QueryResult>, ImgInfo>
{
  std::string controlImgPath;
  ImgInfo imgInfo;

  {
    std::unique_ptr<duckdb::QueryResult> images = analyzer->select(
        "SELECT"
        "  images.image_id as image_id,"
        "  images.width as width,"
        "  images.height as height,"
        "  images.file_name as file_name "
        "FROM images "
        "WHERE"
        " image_id=$1",
        static_cast<uint64_t>(filter.imageId));
    if(images->HasError()) {
      throw std::invalid_argument("S:" + images->GetError());
    }

    auto imageMaterialized = images->Cast<duckdb::StreamQueryResult>().Materialize();
    uint64_t imgWidth      = imageMaterialized->GetValue(1, 0).GetValue<uint64_t>();
    uint64_t imgWeight     = imageMaterialized->GetValue(2, 0).GetValue<uint64_t>();
    controlImgPath         = imageMaterialized->GetValue(3, 0).GetValue<std::string>();

    std::string linkToImage = controlImgPath;
    helper::stringReplace(linkToImage, "${tile_id}", std::to_string(0));

    uint64_t width  = std::ceil(static_cast<float>(imgWidth) / static_cast<float>(filter.heatmapAreaSize));
    uint64_t height = std::ceil(static_cast<float>(imgWeight) / static_cast<float>(filter.heatmapAreaSize));

    imgInfo.width          = width;
    imgInfo.height         = height;
    imgInfo.controlImgPath = linkToImage;
  }

  auto [sql, params]                          = toSqlHeatmap(filter, channelFilter);
  std::unique_ptr<duckdb::QueryResult> result = analyzer->select(sql, params);

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  return {std::move(result), imgInfo};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toSqlHeatmap(const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter)
    -> std::pair<std::string, DbArgs_t>
{
  auto [innerSql, params] = toSqlTable(filter, channelFilter);
  std::string sql         = "WITH innerTable AS (" + innerSql +
                    ")\n"
                    "SELECT\n"
                    "floor(meas_center_x / $4) * $4 AS rectangle_x,\n"
                    "floor(meas_center_y / $4) * $4 AS rectangle_y,\n"
                    "FROM innerTable\n"
                    "GROUP BY floor(meas_center_x / $4), floor(meas_center_y / $4)";
}

}    // namespace joda::db
