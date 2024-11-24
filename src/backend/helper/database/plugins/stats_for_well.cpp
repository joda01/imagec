

#include "stats_for_well.hpp"
#include <cstddef>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

///
/// \class      PositionInWell
/// \author     Joachim Danmayr
/// \brief      Position in well
///
struct ImgPositionInWell
{
  int32_t img = -1;
  int32_t x   = -1;
  int32_t y   = -1;
};

auto transformMatrix(const std::vector<std::vector<int32_t>> &wellImageOrder, int32_t &sizeX, int32_t &sizeY) -> std::map<int32_t, ImgPositionInWell>;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerGroup::toTable(const QueryFilter &filter, Grouping grouping) -> QueryResult
{
  auto classesToExport = filter.getClassesAndClassesToExport();

  for(const auto &[classs, statement] : classesToExport) {
    auto materializedResult =
        getData(classs, filter.getAnalyzer(), filter.getFilter(), statement, grouping)->Cast<duckdb::StreamQueryResult>().Materialize();
    size_t columnNr = statement.getColSize();

    for(size_t row = 0; row < materializedResult->RowCount(); row++) {
      try {
        auto groupId     = materializedResult->GetValue(columnNr + 0, row).GetValue<uint16_t>();
        auto imgGroupIdx = materializedResult->GetValue(columnNr + 1, row).GetValue<uint32_t>();
        auto platePosX   = materializedResult->GetValue(columnNr + 2, row).GetValue<uint32_t>();
        auto platePosY   = materializedResult->GetValue(columnNr + 3, row).GetValue<uint32_t>();
        auto filename    = materializedResult->GetValue(columnNr + 4, row).GetValue<std::string>();
        auto imageId     = materializedResult->GetValue(columnNr + 5, row).GetValue<uint64_t>();
        auto validity    = materializedResult->GetValue(columnNr + 6, row).GetValue<uint64_t>();

        for(int32_t colIdx = 0; colIdx < columnNr; colIdx++) {
          double value = materializedResult->GetValue(colIdx, row).GetValue<double>();
          if(grouping == Grouping::BY_WELL) {
            classesToExport.setData(classs, statement.getColNames(), row, colIdx, filename, table::TableCell{value, imageId, validity == 0, ""});
          } else {
            auto colC = std::string(1, ((char) platePosY + 'A')) + std::to_string(platePosX);
            classesToExport.setData(classs, statement.getColNames(), row, colIdx, colC, table::TableCell{value, groupId, validity == 0, ""});
          }
        }

      } catch(const duckdb::InternalException &) {
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
auto StatsPerGroup::toHeatmap(const QueryFilter &filter, Grouping grouping) -> QueryResult
{
  auto classesToExport = filter.getClassesAndClassesToExport();
  classesToExport.clearTables();

  int32_t sizeX = 0;
  int32_t sizeY = 0;
  std::map<int32_t, ImgPositionInWell> wellPos;
  if(grouping == Grouping::BY_WELL) {
    wellPos = transformMatrix(filter.getFilter().wellImageOrder, sizeX, sizeY);
  } else {
    sizeX = filter.getFilter().plateCols;
    sizeY = filter.getFilter().plateRows;
  }

  for(const auto &[classs, statement] : classesToExport) {
    auto materializedResult =
        getData(classs, filter.getAnalyzer(), filter.getFilter(), statement, grouping)->Cast<duckdb::StreamQueryResult>().Materialize();

    size_t columnNr = statement.getColSize();

    for(size_t row = 0; row < materializedResult->RowCount(); row++) {
      try {
        auto groupId     = materializedResult->GetValue(columnNr + 0, row).GetValue<uint16_t>();
        auto imgGroupIdx = materializedResult->GetValue(columnNr + 1, row).GetValue<uint32_t>();
        auto platePosX   = materializedResult->GetValue(columnNr + 2, row).GetValue<uint32_t>();
        auto platePosY   = materializedResult->GetValue(columnNr + 3, row).GetValue<uint32_t>();
        auto filename    = materializedResult->GetValue(columnNr + 4, row).GetValue<std::string>();
        auto imageId     = materializedResult->GetValue(columnNr + 5, row).GetValue<uint64_t>();
        auto validity    = materializedResult->GetValue(columnNr + 6, row).GetValue<uint64_t>();
        ImgPositionInWell pos;
        if(grouping == Grouping::BY_WELL) {
          pos = wellPos[imgGroupIdx];
        } else {
          if(platePosX > 0) {
            pos.x = --platePosX;
          }
          if(platePosY > 0) {
            pos.y = --platePosY;
          }
        }

        for(size_t col = 0; col < columnNr; col++) {
          double value = materializedResult->GetValue(col, row).GetValue<double>();
          if(grouping == Grouping::BY_WELL) {
            classesToExport.setData(classs, statement.getColNames(), col, pos.y, pos.x, table::TableCell{value, imageId, validity == 0, filename},
                                    sizeX, sizeY, statement.getColumnAt(col).createHeader());
          } else {
            classesToExport.setData(classs, statement.getColNames(), col, pos.y, pos.x, table::TableCell{value, groupId, validity == 0, filename},
                                    sizeX, sizeY, statement.getColumnAt(col).createHeader());
          }
        }

      } catch(const duckdb::InternalException &) {
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
auto StatsPerGroup::getData(const db::ResultingTable::QueryKey &classsAndClass, db::Database *analyzer, const QueryFilter::ObjectFilter &filter,
                            const PreparedStatement &channelFilter, Grouping grouping) -> std::unique_ptr<duckdb::QueryResult>
{
  auto [sql, params] = toSQL(classsAndClass, filter, channelFilter, grouping);

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
auto StatsPerGroup::toSQL(const db::ResultingTable::QueryKey &classsAndClass, const QueryFilter::ObjectFilter &filter,
                          const PreparedStatement &channelFilter, Grouping grouping) -> std::pair<std::string, DbArgs_t>
{
  std::string sql =
      "WITH imageGrouped as (\n"
      "SELECT\n" +
      channelFilter.createStatsQuery(false, false) +
      "	ANY_VALUE(images_groups.group_id) as group_id,\n"
      "	ANY_VALUE(images_groups.image_group_idx) as image_group_idx,\n"
      "	ANY_VALUE(groups.pos_on_plate_x) as pos_on_plate_x,\n"
      "	ANY_VALUE(groups.pos_on_plate_y) as pos_on_plate_y,\n"
      "	ANY_VALUE(images.file_name) as file_name,\n"
      "	ANY_VALUE(images.image_id) as image_id,\n"
      "	MAX(images.validity) as validity\n"
      "FROM\n"
      "	objects t1\n" +
      channelFilter.createStatsQueryJoins() +
      "JOIN images_groups ON\n"
      "	t1.image_id = images_groups.image_id\n"
      "JOIN groups on\n"
      "	images_groups.group_id = groups.group_id\n"
      "JOIN images on\n"
      "	t1.image_id = images.image_id\n";
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

  if(grouping == Grouping::BY_PLATE) {
    sql += channelFilter.createStatsQuery(true, true, grouping == Grouping::BY_WELL ? enums::Stats::OFF : enums::Stats::AVG) +
           " ANY_VALUE(imageGrouped.group_id) as group_id,\n"
           " ANY_VALUE(imageGrouped.image_group_idx) as image_group_idx,\n"
           " ANY_VALUE(imageGrouped.pos_on_plate_x) as pos_on_plate_x,\n"
           " ANY_VALUE(imageGrouped.pos_on_plate_y) as pos_on_plate_y,\n"
           " ANY_VALUE(imageGrouped.file_name) as file_name,\n"
           " ANY_VALUE(imageGrouped.image_id) as image_id,\n"
           " MAX(imageGrouped.validity) as validity\n";
  } else {
    sql += "*";
  }
  sql += "FROM imageGrouped\n";
  if(grouping == Grouping::BY_PLATE) {
    sql += "GROUP BY group_id\n";
    sql += "ORDER BY pos_on_plate_x, pos_on_plate_y\n";
  } else {
    sql += "ORDER BY file_name";
  }

  if(grouping == Grouping::BY_WELL) {
    return {sql,
            {static_cast<uint16_t>(classsAndClass.classs), static_cast<uint16_t>(filter.groupId), static_cast<int32_t>(classsAndClass.zStack),
             static_cast<int32_t>(classsAndClass.tStack)}};
  }
  return {sql,
          {static_cast<uint16_t>(classsAndClass.classs), static_cast<int32_t>(classsAndClass.zStack), static_cast<int32_t>(classsAndClass.tStack)}};
}

///
/// \brief      Transforms a 2D Matrix where the elements in the matrix represents an images index
///             and the coordinates of the matrix the position on the well to a map
///             whereby the key is the images index and the values are the coordinates
///              | 0  1  2
///             -|---------
///             0| 1  2  3
///             1| 4  5  6
///             2| 7  8  9
///
///            [1] => {0,0}
///            [2] => {1,0}
///            ...
///            [9] => {2,2}
///
///
/// \author     Joachim Danmayr
///
auto transformMatrix(const std::vector<std::vector<int32_t>> &wellImageOrder, int32_t &sizeX, int32_t &sizeY) -> std::map<int32_t, ImgPositionInWell>
{
  sizeY = wellImageOrder.size();
  sizeX = 0;

  std::map<int32_t, ImgPositionInWell> ret;
  for(int y = 0; y < wellImageOrder.size(); y++) {
    for(int x = 0; x < wellImageOrder[y].size(); x++) {
      auto imgNr = wellImageOrder[y][x];
      ret[imgNr] = ImgPositionInWell{.img = imgNr, .x = x, .y = y};
      if(x > sizeX) {
        sizeX = x;
      }
    }
  }
  sizeX++;    // Because we start with zro to count
  return ret;
}

}    // namespace joda::db
