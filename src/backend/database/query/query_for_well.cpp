

#include "query_for_well.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>
#include "backend/database/query/filter.hpp"
#include "backend/enums/bigtypes.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerGroup::toTable(db::Database *database, const settings::ResultsSettings &filterIn, Grouping grouping,
                            settings::ResultsSettings *resultingFilter) -> QueryResult
{
  //
  // Remove object IDs, and position since they make no sense in an overview
  //
  settings::ResultsSettings filter;
  uint32_t colIdxOut = 0;
  for(const auto &[_, key] : filterIn.getColumns()) {
    if(settings::ResultsSettings::getType(key.measureChannel) == settings::ResultsSettings::MeasureType::DISTANCE_ID ||
       settings::ResultsSettings::getType(key.measureChannel) == settings::ResultsSettings::MeasureType::ID ||
       settings::ResultsSettings::getType(key.measureChannel) == settings::ResultsSettings::MeasureType::POSITION) {
      continue;
    }
    filter.addColumn({colIdxOut}, key, key.names);
    colIdxOut++;
  }
  filter.setFilter(filterIn.getFilter(), filterIn.getPlateSetup(), filterIn.getDensityMapSettings());
  if(resultingFilter != nullptr) {
    *resultingFilter = filter;
  }

  //
  // Prepare for Image to well place assignment
  //
  int32_t sizeX = 0;
  int32_t sizeY = 0;
  std::map<int32_t, joda::settings::ImgPositionInWell> wellPos;
  if(grouping == Grouping::BY_WELL) {
    wellPos = joda::settings::transformMatrix(filter.getPlateSetup().wellImageOrder, sizeX, sizeY);
  } else {
    sizeX = filter.getPlateSetup().cols;
    sizeY = filter.getPlateSetup().rows;
  }

  //
  // Generate exports
  //
  auto classesToExport = ResultingTable(&filter);

  std::map<stdi::uint128_t, uint32_t> rowIndexes;    // <ID, rowIdx>

  auto findMaxRowIdx = [&rowIndexes]() -> int32_t {
    int32_t rowIdx = -1;
    for(const auto &[_, row] : rowIndexes) {
      if(static_cast<int32_t>(row) > rowIdx) {
        rowIdx = static_cast<int32_t>(row);
      }
    }
    return rowIdx;
  };

  for(const auto &[classs, statement] : classesToExport) {
    auto materializedResult = getData(classs, database, filter.getFilter(), statement, grouping)->Cast<duckdb::StreamQueryResult>().Materialize();
    size_t columnNr         = statement.getColSize();

    for(size_t row = 0; row < materializedResult->RowCount(); row++) {
      try {
        auto groupId     = materializedResult->GetValue(columnNr + 0, row).GetValue<uint16_t>();
        auto imgGroupIdx = materializedResult->GetValue(columnNr + 1, row).GetValue<uint32_t>();
        auto platePosX   = materializedResult->GetValue(columnNr + 2, row).GetValue<uint32_t>();
        auto platePosY   = materializedResult->GetValue(columnNr + 3, row).GetValue<uint32_t>();
        auto filename    = materializedResult->GetValue(columnNr + 4, row).GetValue<std::string>();
        auto imageId     = materializedResult->GetValue(columnNr + 5, row).GetValue<uint64_t>();
        auto validity    = materializedResult->GetValue(columnNr + 6, row).GetValue<uint64_t>();
        auto tStack      = materializedResult->GetValue(columnNr + 7, row).GetValue<uint32_t>();
        size_t rowIdx    = row;
        std::string colC;
        if(grouping == Grouping::BY_WELL) {
          // It could be that there are classes without data, but we have to keep the row order, else the data would be shown shifted and beside a
          // wrong image
          if(rowIndexes.contains({imageId, tStack})) {
            rowIdx = rowIndexes.at({imageId, tStack});
          } else {
            rowIdx = static_cast<size_t>(findMaxRowIdx()) + 1;
            rowIndexes.emplace(stdi::uint128_t{imageId, tStack}, rowIdx);
          }
        } else {
          // It could be that there are classes without data, but we have to keep the row order, else the data would be shown shifted and beside a
          // wrong image
          if(rowIndexes.contains({groupId, tStack})) {
            rowIdx = rowIndexes.at({groupId, tStack});
          } else {
            rowIdx = static_cast<size_t>(findMaxRowIdx()) + 1;
            rowIndexes.emplace(stdi::uint128_t{groupId, tStack}, rowIdx);
          }
          colC = std::string(1, (static_cast<char>(platePosY - 1) + 'A')) + std::to_string(platePosX);
        }

        std::string fileNameTmp;
        if(grouping == Grouping::BY_WELL) {
          fileNameTmp = "t=" + std::to_string(tStack) + " " + filename;
          classesToExport.setRowID(classs, statement.getColNames(), static_cast<int32_t>(rowIdx), fileNameTmp, imageId);
        } else {
          fileNameTmp = "t=" + std::to_string(tStack) + " " + colC;
          classesToExport.setRowID(classs, statement.getColNames(), static_cast<int32_t>(rowIdx), colC, groupId);
        }

        for(int32_t colIdxI = 0; colIdxI < static_cast<int32_t>(columnNr); colIdxI++) {
          double value = materializedResult->GetValue(static_cast<uint32_t>(colIdxI), row).GetValue<double>();
          if(grouping == Grouping::BY_WELL) {
            ///
            joda::settings::ImgPositionInWell pos;
            if(wellPos.contains(static_cast<int32_t>(imgGroupIdx))) {
              pos = wellPos[static_cast<int32_t>(imgGroupIdx)];
            } else {
              pos.x = 1;
              pos.y = 1;
            }
            classesToExport.setData(classs, statement.getColNames(), static_cast<uint32_t>(rowIdx), static_cast<uint32_t>(colIdxI), fileNameTmp,
                                    table::TableCell{value,
                                                     table::TableCell::MetaData{.objectIdGroup  = imageId,
                                                                                .objectId       = imageId,
                                                                                .parentObjectId = 0,
                                                                                .trackingId     = 0,
                                                                                .isValid        = validity == 0,
                                                                                .tStack         = tStack,
                                                                                .zStack         = 0,
                                                                                .cStack         = 0,
                                                                                .rowName        = fileNameTmp},
                                                     table::TableCell::Grouping{.groupIdx = static_cast<uint64_t>(imgGroupIdx),
                                                                                .posX     = static_cast<uint32_t>(pos.x),
                                                                                .posY     = static_cast<uint32_t>(pos.y)}});
          } else {
            classesToExport.setData(
                classs, statement.getColNames(), static_cast<uint32_t>(rowIdx), static_cast<uint32_t>(colIdxI), fileNameTmp,
                table::TableCell{value,
                                 table::TableCell::MetaData{.objectIdGroup  = groupId,
                                                            .objectId       = groupId,
                                                            .parentObjectId = 0,
                                                            .trackingId     = 0,
                                                            .isValid        = validity == 0,
                                                            .tStack         = tStack,
                                                            .zStack         = 0,
                                                            .cStack         = 0,
                                                            .rowName        = fileNameTmp},
                                 table::TableCell::Grouping{.groupIdx = static_cast<uint64_t>((static_cast<uint64_t>(platePosX) << 32) | platePosY),
                                                            .posX     = platePosX,
                                                            .posY     = platePosY}});
          }
        }
      } catch(const duckdb::InternalException &ex) {
        std::cout << ex.what() << std::endl;
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
auto StatsPerGroup::getData(const db::ResultingTable::QueryKey &classsAndClass, db::Database *analyzer,
                            const settings::ResultsSettings::ObjectFilter &filter, const PreparedStatement &channelFilter, Grouping grouping)
    -> std::unique_ptr<duckdb::QueryResult>
{
  auto [sql, params]                          = toSQL(classsAndClass, filter, channelFilter, grouping);
  std::unique_ptr<duckdb::QueryResult> result = analyzer->select(sql, params);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
  return result;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerGroup::toSQL(const db::ResultingTable::QueryKey &classsAndClass, const settings::ResultsSettings::ObjectFilter &filter,
                          const PreparedStatement &channelFilter, Grouping grouping) -> std::pair<std::string, DbArgs_t>
{
  auto [retValSum, retValCnt] = channelFilter.createIntersectionQuery();
  std::string intersect;

  if(!retValSum.empty()) {
    intersect =
        " WITH TblIntersecting as (\n"
        "  SELECT\n"
        "  ad.image_id,\n" +
        retValSum +
        "  FROM objects ad,\n"
        "  WHERE ad.meas_parent_class_id=" +
        std::to_string(static_cast<uint16_t>(classsAndClass.classs)) +
        "  GROUP BY ad.image_id, ad.class_id, ad.meas_parent_object_id, ad.meas_parent_class_id\n"
        "),\n";
  } else {
    intersect = "WITH ";
  }
  std::string sql = intersect +
                    "imageGrouped as (\n"
                    "SELECT\n" +
                    channelFilter.createStatsQuery(false, false) +
                    "	ANY_VALUE(images_groups.group_id) as group_id,\n"
                    "	ANY_VALUE(images_groups.image_group_idx) as image_group_idx,\n"
                    "	ANY_VALUE(groups.pos_on_plate_x) as pos_on_plate_x,\n"
                    "	ANY_VALUE(groups.pos_on_plate_y) as pos_on_plate_y,\n"
                    "	ANY_VALUE(images.file_name) as file_name,\n"
                    "	ANY_VALUE(images.image_id) as image_id,\n"
                    "	MAX(images.validity) as validity,\n"
                    "	ANY_VALUE(t1.stack_t) as stack_t_real\n"
                    "FROM objects t1\n" +
                    channelFilter.createStatsQueryJoins(false) +
                    " JOIN images_groups ON\n"
                    "	t1.image_id = images_groups.image_id\n"
                    "JOIN groups on\n"
                    "	images_groups.group_id = groups.group_id\n"
                    "JOIN images on\n"
                    "	t1.image_id = images.image_id\n";

  // Select exact one T-Stack
  if(filter.tStackHandling == settings::ResultsSettings::ObjectFilter::TStackHandling::INDIVIDUAL) {
    if(grouping == Grouping::BY_WELL) {
      sql += "WHERE\n";
      sql += " t1.class_id=$1 AND images_groups.group_id=$2 AND stack_z=$3 AND stack_t=$4\n";
    } else {
      sql += "WHERE\n";
      sql += " t1.class_id=$1 AND stack_z=$2 AND stack_t=$3\n";
    }
    sql +=
        "GROUP BY\n"
        "	t1.image_id\n"
        ")\n"
        "SELECT\n";
  } else if(filter.tStackHandling == settings::ResultsSettings::ObjectFilter::TStackHandling::SLICE) {
    if(grouping == Grouping::BY_WELL) {
      sql += "WHERE\n";
      sql += " t1.class_id=$1 AND images_groups.group_id=$2 AND stack_z=$3\n";
    } else {
      sql += "WHERE\n";
      sql += " t1.class_id=$1 AND stack_z=$2\n";
    }

    sql +=
        "GROUP BY\n"
        "	t1.image_id, t1.stack_t\n"
        ")\n"
        "SELECT\n";
  }

  if(grouping == Grouping::BY_PLATE) {
    sql += channelFilter.createStatsQuery(true, true, "ANY_VALUE", grouping == Grouping::BY_WELL ? enums::Stats::OFF : enums::Stats::AVG) +
           " ANY_VALUE(imageGrouped.group_id) as group_id,\n"
           " ANY_VALUE(imageGrouped.image_group_idx) as image_group_idx,\n"
           " ANY_VALUE(imageGrouped.pos_on_plate_x) as pos_on_plate_x,\n"
           " ANY_VALUE(imageGrouped.pos_on_plate_y) as pos_on_plate_y,\n"
           " ANY_VALUE(imageGrouped.file_name) as file_name,\n"
           " ANY_VALUE(imageGrouped.image_id) as image_id,\n"
           " MAX(imageGrouped.validity) as validity,\n"
           " ANY_VALUE(imageGrouped.stack_t_real) as stack_t_real\n";
  } else {
    sql += "*";
  }
  sql += "FROM imageGrouped\n";

  if(filter.tStackHandling == settings::ResultsSettings::ObjectFilter::TStackHandling::INDIVIDUAL) {
    if(grouping == Grouping::BY_PLATE) {
      sql += "GROUP BY group_id\n";
      sql += "ORDER BY pos_on_plate_y, pos_on_plate_x\n";
    } else {
      sql += "ORDER BY file_name";
    }
  } else if(filter.tStackHandling == settings::ResultsSettings::ObjectFilter::TStackHandling::SLICE) {
    if(grouping == Grouping::BY_PLATE) {
      sql += "GROUP BY group_id,stack_t_real\n";
      sql += "ORDER BY pos_on_plate_y, pos_on_plate_x,stack_t_real\n";
    } else {
      sql += "ORDER BY file_name,stack_t_real";
    }
  }

  if(filter.tStackHandling == settings::ResultsSettings::ObjectFilter::TStackHandling::INDIVIDUAL) {
    if(grouping == Grouping::BY_WELL) {
      return {sql,
              {static_cast<uint16_t>(classsAndClass.classs), static_cast<uint16_t>(filter.groupId), static_cast<int32_t>(classsAndClass.zStack),
               static_cast<int32_t>(classsAndClass.tStack)}};
    }
    return {sql,
            {static_cast<uint16_t>(classsAndClass.classs), static_cast<int32_t>(classsAndClass.zStack), static_cast<int32_t>(classsAndClass.tStack)}};
  } else if(filter.tStackHandling == settings::ResultsSettings::ObjectFilter::TStackHandling::SLICE) {
    if(grouping == Grouping::BY_WELL) {
      return {sql,
              {static_cast<uint16_t>(classsAndClass.classs), static_cast<uint16_t>(filter.groupId), static_cast<int32_t>(classsAndClass.zStack)}};
    }
    return {sql, {static_cast<uint16_t>(classsAndClass.classs), static_cast<int32_t>(classsAndClass.zStack)}};
  }
  throw std::invalid_argument("Unknow t-Stack handling");
}

}    // namespace joda::db
