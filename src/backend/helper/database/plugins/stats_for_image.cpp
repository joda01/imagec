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
#include <exception>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/logger/console_logger.hpp"

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
    //
    // This is used to align the objects for each class correct
    //
    std::map<uint64_t, int32_t> rowIndexes;    // <ID, rowIdx>
    auto findMaxRowIdx = [&rowIndexes]() -> int32_t {
      int32_t rowIdx = -1;
      for(const auto &[_, row] : rowIndexes) {
        if(row > rowIdx) {
          rowIdx = row;
        }
      }
      return rowIdx;
    };

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
          uint32_t meas_center_x = materializedResult->GetValue(columnNr + 0, row).GetValue<uint32_t>();
          uint32_t meas_center_y = materializedResult->GetValue(columnNr + 1, row).GetValue<uint32_t>();
          uint64_t objectId      = materializedResult->GetValue(columnNr + 2, row).GetValue<uint64_t>();

          // It could be that there are classes without data, but we have to keep the row order, else the data would be shown shifted and beside a
          // wrong image
          size_t rowIdx = row;
          if(rowIndexes.contains(objectId)) {
            rowIdx = rowIndexes.at(objectId);
          } else {
            rowIdx = findMaxRowIdx() + 1;
            rowIndexes.emplace(objectId, rowIdx);
          }

          double value = materializedResult->GetValue(colIdx, row).GetValue<double>();

          classesToExport.setData(classs, statement.getColNames(), rowIdx, colIdx, std::to_string(rowIdx),
                                  table::TableCell{value, objectId, true, ""});
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
  auto [retValSum, retValCnt] = channelFilter.createIntersectionQuery();
  std::string intersect;
  std::string table = "objects";

  if(!retValSum.empty()) {
    table = "Intermediate";
    intersect =
        " WITH RECURSIVE AllDescendants AS (\n"
        "  SELECT\n"
        "    object_id,\n"
        "    meas_parent_object_id,\n"
        "    object_id AS root_id,\n"
        "    class_id\n"
        "  FROM objects\n"
        "  UNION ALL\n"
        "  SELECT\n"
        "    t.object_id,\n"
        "    t.meas_parent_object_id,\n"
        "    ad.root_id,\n"
        "    t.class_id\n"
        "  FROM objects t\n"
        "  JOIN AllDescendants ad\n"
        "    ON t.meas_parent_object_id = ad.object_id\n"
        "),\n"
        "RootClasses AS (\n"
        "  SELECT\n"
        "    object_id AS root_id,\n"
        "    class_id AS root_class_id\n"
        "  FROM objects\n"
        "),\n"
        "DescendantCounts AS (\n"
        "  SELECT\n"
        "    ad.root_id,\n" +
        retValSum +
        "  FROM AllDescendants ad\n"
        "  GROUP BY ad.root_id\n"
        "),\n"
        "Intermediate AS(\n"
        "  SELECT\n"
        "    t.*,\n" +
        retValCnt +
        "  FROM objects t\n"
        "  LEFT JOIN DescendantCounts dc\n"
        "    ON t.object_id = dc.root_id\n"
        "  LEFT JOIN RootClasses rc\n"
        "    ON t.object_id = rc.root_id\n"
        ")\n";
  } else {
    intersect = "";
  }
  std::string sql = intersect + "SELECT\n" + channelFilter.createStatsQuery(false, false) +
                    "ANY_VALUE(t1.meas_center_x) as meas_center_x,\n"
                    "ANY_VALUE(t1.meas_center_y) as meas_center_y,\n"
                    "ANY_VALUE(t1.object_id) as object_id\n"
                    "FROM\n"
                    "  " +
                    table + " t1\n" + channelFilter.createStatsQueryJoins() +
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
    try {
      auto imageMaterialized = images->Cast<duckdb::StreamQueryResult>().Materialize();
      if(!imageMaterialized->GetValue(1, 0).IsNull()) {
        uint64_t imgWidth  = imageMaterialized->GetValue(1, 0).GetValue<uint64_t>();
        uint64_t imgWeight = imageMaterialized->GetValue(2, 0).GetValue<uint64_t>();
        controlImgPath     = imageMaterialized->GetValue(3, 0).GetValue<std::string>();

        std::string linkToImage = controlImgPath;
        helper::stringReplace(linkToImage, "${tile_id}", std::to_string(0));

        uint64_t width  = std::ceil(static_cast<float>(imgWidth) / static_cast<float>(filter.densityMapAreaSize));
        uint64_t height = std::ceil(static_cast<float>(imgWeight) / static_cast<float>(filter.densityMapAreaSize));

        imgInfo.width          = width;
        imgInfo.height         = height;
        imgInfo.controlImgPath = linkToImage;
      } else {
        imgInfo.width          = 0;
        imgInfo.height         = 0;
        imgInfo.controlImgPath = "";
      }
    } catch(const std::exception &ex) {
      joda::log::logWarning(ex.what());
      imgInfo.width          = 0;
      imgInfo.height         = 0;
      imgInfo.controlImgPath = "";
    }
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
