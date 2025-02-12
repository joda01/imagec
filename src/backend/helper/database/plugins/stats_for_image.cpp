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

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toTable(db::Database *database, const QueryFilter &filter) -> QueryResult
{
  auto classesToExport = filter.getClassesToExport();

  for(const auto &[classs, statement] : classesToExport) {
    auto [sql, params] = toSqlTable(classs, filter.getFilter(), statement);
    auto result        = database->select(sql, params);

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    size_t columnNr         = statement.getColSize();
    for(size_t row = 0; row < materializedResult->RowCount(); row++) {
      for(int32_t colIdx = 0; colIdx < columnNr; colIdx++) {
        if(!materializedResult->GetValue(colIdx, row).IsNull()) {
          double value = materializedResult->GetValue(colIdx, row).GetValue<double>();
          classesToExport.setData(classs, statement.getColNames(), row, colIdx, std::to_string(row), table::TableCell{value, 0, true, ""});
        }
      }
    }
  }
  return classesToExport.getResult();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toSqlTable(const db::ResultingTable::QueryKey &classsAndClass, const QueryFilter::ObjectFilter &filter,
                               const PreparedStatement &channelFilter) -> std::pair<std::string, DbArgs_t>
{
  std::string sql = "SELECT\n" + channelFilter.createStatsQuery(false, false) +
                    "ANY_VALUE(t1.meas_center_x) as meas_center_x,\n"
                    "ANY_VALUE(t1.meas_center_y) as meas_center_y\n"
                    "FROM\n"
                    "  objects t1\n" +
                    channelFilter.createStatsQueryJoins() +
                    "WHERE\n"
                    " t1.image_id=$1 AND t1.class_id=$2 AND stack_z=$3 AND stack_t=$4\n"
                    "GROUP BY t1.object_id\n"
                    "ORDER BY t1.object_id";
  return {sql,
          {filter.imageId, static_cast<uint16_t>(classsAndClass.classs), static_cast<int32_t>(classsAndClass.zStack),
           static_cast<int32_t>(classsAndClass.tStack)}};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toHeatmap(db::Database *database, const QueryFilter &filter) -> QueryResult
{
  auto classesToExport = filter.getClassesToExport();

  int32_t tabIdx = 0;
  for(const auto &[classs, statement] : classesToExport) {
    auto [result, imageInfo] = densityMap(classs, database, filter.getFilter(), statement);
    size_t columnNr          = statement.getColSize();
    auto materializedResult  = result->Cast<duckdb::StreamQueryResult>().Materialize();

    for(size_t colIdx = 0; colIdx < columnNr; colIdx++) {
      auto generateHeatmap = [&columnNr, &tabIdx, &classesToExport, &filter, &materializedResult,
                              imageInfo = imageInfo](int32_t colIdx, const PreparedStatement &statement) {
        joda::table::Table &results = classesToExport.getTable(tabIdx);
        results.setTitle(statement.getColumnAt(colIdx).createHeader());
        results.setMeta({statement.getColNames().className});

        for(uint64_t row = 0; row < imageInfo.height; row++) {
          results.getMutableRowHeader()[row] = std::to_string(row + 1);
          for(uint64_t col = 0; col < imageInfo.width; col++) {
            results.getMutableColHeader()[col] = std::to_string(col + 1);
            results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, imageInfo.controlImgPath});
          }
        }
        for(size_t n = 0; n < materializedResult->RowCount(); n++) {
          try {
            double value        = materializedResult->GetValue(colIdx, n).GetValue<double>();
            uint32_t rectangleX = materializedResult->GetValue(columnNr + 0, n).GetValue<double>();
            uint32_t rectangleY = materializedResult->GetValue(columnNr + 1, n).GetValue<double>();

            uint32_t x = rectangleX / filter.getFilter().densityMapAreaSize;
            uint32_t y = rectangleY / filter.getFilter().densityMapAreaSize;

            std::string linkToImage = imageInfo.controlImgPath;
            results.setData(y, x, table::TableCell{value, 0, true, linkToImage});

          } catch(const duckdb::InternalException &ex) {
            std::cout << "EX " << ex.what() << std::endl;
          }
        }
      };
      generateHeatmap(colIdx, statement);
      tabIdx++;
    }
  }
  return classesToExport.getResult();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::densityMap(const db::ResultingTable::QueryKey &classsAndClass, db::Database *analyzer, const QueryFilter::ObjectFilter &filter,
                               const PreparedStatement &channelFilter) -> std::tuple<std::unique_ptr<duckdb::QueryResult>, ImgInfo>
{
  std::string controlImgPath;
  ImgInfo imgInfo;

  {
    std::unique_ptr<duckdb::QueryResult> images = analyzer->select(
        "SELECT\n"
        "  images.image_id as image_id,\n"
        "  images.width as width,\n"
        "  images.height as height,\n"
        "  images.file_name as file_name\n"
        "FROM images\n"
        "WHERE\n"
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

    uint64_t width  = std::ceil(static_cast<float>(imgWidth) / static_cast<float>(filter.densityMapAreaSize));
    uint64_t height = std::ceil(static_cast<float>(imgWeight) / static_cast<float>(filter.densityMapAreaSize));

    imgInfo.width          = width;
    imgInfo.height         = height;
    imgInfo.controlImgPath = linkToImage;
  }

  auto [sql, params]                          = toSqlHeatmap(classsAndClass, filter, channelFilter);
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
auto StatsPerImage::toSqlHeatmap(const db::ResultingTable::QueryKey &classsAndClass, const QueryFilter::ObjectFilter &filter,
                                 const PreparedStatement &channelFilter) -> std::pair<std::string, DbArgs_t>
{
  auto [innerSql, params] = toSqlTable(classsAndClass, filter, channelFilter);
  std::string sql         = "WITH innerTable AS (\n" + innerSql +
                    "\n)\n"
                    "SELECT\n" +
                    channelFilter.createStatsQuery(true, false) +
                    "floor(meas_center_x / $5) * $5 AS rectangle_x,\n"
                    "floor(meas_center_y / $5) * $5 AS rectangle_y,\n"
                    "FROM innerTable\n"
                    "GROUP BY floor(meas_center_x / $5), floor(meas_center_y / $5)";

  params.emplace_back(static_cast<double>(filter.densityMapAreaSize));
  return {sql, params};
}

}    // namespace joda::db
