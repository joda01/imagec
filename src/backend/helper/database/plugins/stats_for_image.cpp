///
/// \file      stats_for_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "stats_for_image.hpp"
#include <exception>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toTable(db::Database *database, const settings::ResultsSettings &filterIn, settings::ResultsSettings *resultingFilter)
    -> QueryResult
{
  //
  // Remove all stats but one per channel since one image level an average e.g. for just one object does not make sense
  //
  settings::ResultsSettings filter;
  std::set<settings::ResultsSettings::ColumnKey> stillMeasured;
  std::map<int32_t, int32_t> tabColIdx;
  for(const auto &[_, key] : filterIn.getColumns()) {
    settings::ResultsSettings::ColumnKey keyTmp = key;
    keyTmp.stats                                = enums::Stats::OFF;
    if(!stillMeasured.contains(keyTmp)) {
      if(!tabColIdx.contains(_.tabIdx)) {
        tabColIdx.emplace(_.tabIdx, 0);
      } else {
        tabColIdx[_.tabIdx]++;
      }

      filter.addColumn({_.tabIdx, tabColIdx[_.tabIdx]}, keyTmp, key.names);
      stillMeasured.emplace(keyTmp);
    }
  }
  filter.setFilter(filterIn.getFilter(), filterIn.getPlateSetup(), filterIn.getDensityMapSettings());
  if(resultingFilter != nullptr) {
    *resultingFilter = filter;
  }

  //
  // Generate exports
  //
  auto classesToExport = ResultingTable(&filter);

  auto findMaxRowIdx = [](const std::map<uint64_t, int32_t> &rowIndexes) -> int32_t {
    int32_t rowIdx = -1;
    for(const auto &[_, row] : rowIndexes) {
      if(row > rowIdx) {
        rowIdx = row;
      }
    }
    return rowIdx;
  };

  //
  // This is used to align the objects for each class correct
  //
  for(const auto &[classs, statement] : classesToExport) {
    auto [sql, params] = toSqlTable(classs, filter.getFilter(), statement, "");
    auto result        = database->select(sql, params);
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    size_t columnNr         = statement.getColSize();
    for(int32_t rowIdx = 0; rowIdx < materializedResult->RowCount(); rowIdx++) {
      for(int32_t colIdx = 0; colIdx < columnNr; colIdx++) {
        if(!materializedResult->GetValue(colIdx, rowIdx).IsNull()) {
          uint32_t meas_center_x  = materializedResult->GetValue(columnNr + 0, rowIdx).GetValue<uint32_t>();
          uint32_t meas_center_y  = materializedResult->GetValue(columnNr + 1, rowIdx).GetValue<uint32_t>();
          uint64_t objectId       = materializedResult->GetValue(columnNr + 2, rowIdx).GetValue<uint64_t>();
          uint64_t objectIdReal   = materializedResult->GetValue(columnNr + 3, rowIdx).GetValue<uint64_t>();
          uint64_t parentObjectId = materializedResult->GetValue(columnNr + 4, rowIdx).GetValue<uint64_t>();
          auto trackIdTmp         = materializedResult->GetValue(columnNr + 5, rowIdx);
          auto filename           = materializedResult->GetValue(columnNr + 6, rowIdx).GetValue<std::string>();
          uint64_t trackingId     = 0;
          if(!trackIdTmp.IsNull()) {
            trackingId = trackIdTmp.GetValue<uint64_t>();
          }
          double value = materializedResult->GetValue(colIdx, rowIdx).GetValue<double>();
          classesToExport.setData(classs, statement.getColNames(), rowIdx, colIdx, filename,
                                  table::TableCell{value, objectId, objectIdReal, true, parentObjectId, trackingId});
        }
      }
    }
  }
  for(auto &[_, table] : classesToExport.mutableResult()) {
    table.arrangeByTrackingId();
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
auto StatsPerImage::toSqlTable(const db::ResultingTable::QueryKey &classsAndClass, const settings::ResultsSettings::ObjectFilter &filter,
                               const PreparedStatement &channelFilter, const std::string &offValue) -> std::pair<std::string, DbArgs_t>
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

  std::string query = "(";
  DbArgs_t args;
  int i = 0;
  for(auto imageId : filter.imageId) {
    query += (i > 0 ? ", ?" : "?");
    args.emplace_back(static_cast<uint64_t>(imageId));
    i++;
  }
  query += ")";

  std::string uniqueObjectId = offValue + "(t1.object_id) as object_id,\n";
  if(offValue.empty()) {
    if(classsAndClass.distanceToClass != joda::enums::ClassId::NONE) {
      // For distance measurement it is special since the distance for one object to multiple other objects is measured
      uniqueObjectId = "ROW_NUMBER() OVER (ORDER BY " + offValue + "(t1.object_id)) as object_id,\n";
    }
  }

  std::string grouping;
  if(!offValue.empty()) {
    grouping = "GROUP BY t1.object_id\n";
  }
  std::string sql = intersect + "SELECT\n" + channelFilter.createStatsQuery(false, false, offValue) + offValue +
                    "(t1.meas_center_x) as meas_center_x,\n" + offValue + "(t1.meas_center_y) as meas_center_y,\n" + uniqueObjectId + offValue +
                    "(t1.object_id) as object_id_real,\n" + "(t1.meas_parent_object_id) as meas_parent_object_id,\n" + offValue +
                    "(t1.meas_tracking_id) as meas_tracking_id,\n" + offValue + "(images.file_name) as file_name\n" +
                    "FROM\n"
                    "  " +
                    table + " t1\n" + channelFilter.createStatsQueryJoins() +
                    "JOIN images on\n"
                    "	t1.image_id = images.image_id\n"
                    "WHERE\n"
                    " t1.image_id IN" +
                    query + " AND t1.class_id=? AND stack_z=? AND stack_t=?\n" + grouping + "ORDER BY file_name,object_id";

  DbArgs_t argsEnd = {static_cast<uint16_t>(classsAndClass.classs), static_cast<int32_t>(classsAndClass.zStack),
                      static_cast<int32_t>(classsAndClass.tStack)};
  args.insert(args.end(), argsEnd.begin(), argsEnd.end());

  return {sql, args};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerImage::toHeatmap(db::Database *database, const settings::ResultsSettings &filter, settings::ResultsSettings *resultingFilter)
    -> QueryResult
{
  if(resultingFilter != nullptr) {
    *resultingFilter = filter;
  }

  auto classesToExport = ResultingTable(&filter);

  // int32_t tabIdx = 0;
  for(const auto &[classs, statement] : classesToExport) {
    auto [result, imageInfo] = densityMap(classs, database, filter.getFilter(), filter.getDensityMapSettings(), statement);
    size_t columnNr          = statement.getColSize();
    auto materializedResult  = result->Cast<duckdb::StreamQueryResult>().Materialize();

    for(size_t dbColIdx = 0; dbColIdx < columnNr; dbColIdx++) {
      auto tabIdx = classesToExport.getColIdxFromDbColIdx(statement, dbColIdx);

      auto generateHeatmap = [&columnNr, &tabIdx, &classesToExport, &filter, &materializedResult,
                              imageInfo = imageInfo](int32_t dbColIdx, const PreparedStatement &statement) {
        joda::table::Table &results = classesToExport.getTable(tabIdx);

        results.setTitle(statement.getColumnAt(dbColIdx).createHeader());
        results.setMeta({statement.getColNames().className});
        results.setColHeader({{0, statement.getColumnAt(dbColIdx).createHeader()}});

        for(uint64_t row = 0; row < imageInfo.height; row++) {
          results.getMutableRowHeader()[row] = std::to_string(row + 1);
          for(uint64_t col = 0; col < imageInfo.width; col++) {
            results.getMutableColHeader()[col] = std::to_string(col + 1);
            results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, 0, false, imageInfo.controlImgPath});
          }
        }

        for(size_t n = 0; n < materializedResult->RowCount(); n++) {
          try {
            double value        = materializedResult->GetValue(dbColIdx, n).GetValue<double>();
            uint32_t rectangleX = materializedResult->GetValue(columnNr + 0, n).GetValue<double>();
            uint32_t rectangleY = materializedResult->GetValue(columnNr + 1, n).GetValue<double>();

            uint32_t x = rectangleX / filter.getDensityMapSettings().densityMapAreaSize;
            uint32_t y = rectangleY / filter.getDensityMapSettings().densityMapAreaSize;

            std::string linkToImage = imageInfo.controlImgPath;
            results.setData(y, x, table::TableCell{value, 0, 0, true, linkToImage});

          } catch(const duckdb::InternalException &ex) {
            // std::cout << "EX " << ex.what() << std::endl;
          }
        }
      };

      generateHeatmap(dbColIdx, statement);
      // tabIdx++;
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
auto StatsPerImage::densityMap(const db::ResultingTable::QueryKey &classsAndClass, db::Database *analyzer,
                               const settings::ResultsSettings::ObjectFilter &filter, const settings::DensityMapSettings &densityMapSettings,
                               const PreparedStatement &channelFilter) -> std::tuple<std::unique_ptr<duckdb::QueryResult>, ImgInfo>
{
  std::string controlImgPath;
  ImgInfo imgInfo;

  {
    std::string query =
        "SELECT\n"
        "  images.image_id as image_id,\n"
        "  images.width as width,\n"
        "  images.height as height,\n"
        "  images.file_name as file_name\n"
        "FROM images\n"
        "WHERE image_id IN (";

    DbArgs_t args;
    int i = 0;
    for(auto imageId : filter.imageId) {
      query += (i > 0 ? ", ?" : "?");
      args.emplace_back(static_cast<uint64_t>(imageId));
      i++;
    }
    query += ")";

    std::unique_ptr<duckdb::QueryResult> images = analyzer->select(query, args);
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

        uint64_t width  = std::ceil(static_cast<float>(imgWidth) / static_cast<float>(densityMapSettings.densityMapAreaSize));
        uint64_t height = std::ceil(static_cast<float>(imgWeight) / static_cast<float>(densityMapSettings.densityMapAreaSize));

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

  auto [sql, params]                          = toSqlHeatmap(classsAndClass, filter, densityMapSettings, channelFilter);
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
auto StatsPerImage::toSqlHeatmap(const db::ResultingTable::QueryKey &classsAndClass, const settings::ResultsSettings::ObjectFilter &filter,
                                 const settings::DensityMapSettings &densityMapSettings, const PreparedStatement &channelFilter)
    -> std::pair<std::string, DbArgs_t>
{
  auto [innerSql, params] = toSqlTable(classsAndClass, filter, channelFilter);
  std::string placeHolder = "$" + std::to_string(filter.imageId.size() + 4);

  std::string sql = "WITH innerTable AS (\n" + innerSql +
                    "\n)\n"
                    "SELECT\n" +
                    channelFilter.createStatsQuery(true, false) + "floor(meas_center_x / " + placeHolder + ") * " + placeHolder +
                    " AS rectangle_x,\n"
                    "floor(meas_center_y / " +
                    placeHolder + ") * " + placeHolder +
                    " AS rectangle_y,\n"
                    "FROM innerTable\n"
                    "GROUP BY floor(meas_center_x / " +
                    placeHolder + "), floor(meas_center_y / " + placeHolder + ")";

  params.emplace_back(static_cast<double>(densityMapSettings.densityMapAreaSize));
  return {sql, params};
}

}    // namespace joda::db
