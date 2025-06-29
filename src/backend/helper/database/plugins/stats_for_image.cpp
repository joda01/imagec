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
  int32_t tabColIdx = 0;
  for(const auto &[_, key] : filterIn.getColumns()) {
    settings::ResultsSettings::ColumnKey keyTmp = key;
    keyTmp.stats                                = enums::Stats::OFF;
    if(!stillMeasured.contains(keyTmp)) {
      filter.addColumn({tabColIdx}, keyTmp, key.names);
      stillMeasured.emplace(keyTmp);
      tabColIdx++;
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
          auto tStack             = materializedResult->GetValue(columnNr + 7, rowIdx).GetValue<uint32_t>();
          uint64_t trackingId     = 0;
          if(!trackIdTmp.IsNull()) {
            trackingId = trackIdTmp.GetValue<uint64_t>();
          }
          double value            = materializedResult->GetValue(colIdx, rowIdx).GetValue<double>();
          std::string fileNameTmp = "t=" + std::to_string(tStack) + " " + filename;
          classesToExport.setData(classs, statement.getColNames(), rowIdx, colIdx, fileNameTmp,
                                  table::TableCell{value,
                                                   table::TableCell::MetaData{.objectIdGroup  = objectId,
                                                                              .objectId       = objectIdReal,
                                                                              .parentObjectId = parentObjectId,
                                                                              .trackingId     = trackingId,
                                                                              .isValid        = true,
                                                                              .tStack         = tStack,
                                                                              .zStack         = 0,
                                                                              .cStack         = 0,
                                                                              .rowName        = fileNameTmp},
                                                   table::TableCell::Grouping{.groupIdx = static_cast<uint64_t>(
                                                                                  (static_cast<uint64_t>(meas_center_x) << 32) | meas_center_y),
                                                                              .posX = meas_center_x,
                                                                              .posY = meas_center_y}});
        }
      }
    }
  }

  // classesToExport.mutableResult().arrangeByTrackingId();

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

  std::string query = "(";
  DbArgs_t args;
  int i = 0;
  for(auto imageId : filter.imageId) {
    query += (i > 0 ? ", $" + std::to_string(i + 1) : "$" + std::to_string(i + 1));
    args.emplace_back(static_cast<uint64_t>(imageId));
    i++;
  }
  query += ")";

  if(!retValSum.empty()) {
    intersect =
        " WITH TblIntersecting as (\n"
        "  SELECT\n"
        "  image_id,"
        "  ad.meas_parent_object_id as object_id,\n" +
        retValSum +
        "  FROM objects ad,\n"
        "  WHERE ad.image_id IN " +
        query + " AND ad.stack_z=" + std::to_string(static_cast<int32_t>(classsAndClass.zStack)) +
        " AND ad.stack_t=" + std::to_string(static_cast<int32_t>(classsAndClass.tStack)) +
        " AND ad.meas_parent_class_id=" + std::to_string(static_cast<uint16_t>(classsAndClass.classs)) +
        "  GROUP BY ad.image_id, ad.class_id, ad.meas_parent_object_id, ad.meas_parent_class_id\n"
        ")\n";
  } else {
    intersect = "";
  }

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
                    "(t1.object_id) as object_id_real,\n" + offValue + "(t1.meas_parent_object_id) as meas_parent_object_id,\n" + offValue +
                    "(t1.meas_tracking_id) as meas_tracking_id,\n" + offValue + "(images.file_name) as file_name,\n" + offValue +
                    "(t1.stack_t) as stack_t_real\n"
                    "FROM objects t1\n" +
                    channelFilter.createStatsQueryJoins(true) +
                    "JOIN images on\n"
                    "	t1.image_id = images.image_id\n"
                    "WHERE\n"
                    " t1.image_id IN" +
                    query + " AND t1.class_id=$" + std::to_string(i + 1) + " AND stack_z=$" + std::to_string(i + 2) + " AND stack_t=$" +
                    std::to_string(i + 3) + "\n" + grouping + "ORDER BY file_name, meas_parent_object_id, object_id,stack_t_real";
  DbArgs_t argsEnd = {static_cast<uint16_t>(classsAndClass.classs), static_cast<int32_t>(classsAndClass.zStack),
                      static_cast<int32_t>(classsAndClass.tStack)};
  args.insert(args.end(), argsEnd.begin(), argsEnd.end());

  return {sql, args};
}

}    // namespace joda::db
