

#include "stats_for_well.hpp"
#include <cstddef>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/plugins/filter.hpp"

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
  auto clustersToExport = filter.getClustersAndClassesToExport();

  for(const auto &[clusterClass, statement] : clustersToExport) {
    std::cout << "Get data " << std::to_string((uint32_t) clusterClass.clusterId) << "@" << std::to_string((uint32_t) clusterClass.classId)
              << std::endl;

    auto materializedResult =
        getData(clusterClass, filter.getAnalyzer(), filter.getFilter(), statement, grouping)->Cast<duckdb::StreamQueryResult>().Materialize();
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
            clustersToExport.setData(clusterClass, statement.getColNames(), row, colIdx, filename,
                                     table::TableCell{value, imageId, validity == 0, ""});
          } else {
            clustersToExport.setData(clusterClass, statement.getColNames(), row, colIdx, std::to_string(row),
                                     table::TableCell{value, groupId, validity == 0, ""});
          }
        }

      } catch(const duckdb::InternalException &) {
      }
    }
  }
  return clustersToExport.getResult();
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
  auto clustersToExport = filter.getClustersAndClassesToExport();
  clustersToExport.clearTables();

  int32_t sizeX = 0;
  int32_t sizeY = 0;
  std::map<int32_t, ImgPositionInWell> wellPos;
  if(grouping == Grouping::BY_WELL) {
    wellPos = transformMatrix(filter.getFilter().wellImageOrder, sizeX, sizeY);
  } else {
    sizeX = filter.getFilter().plateCols;
    sizeY = filter.getFilter().plateRows;
  }

  for(const auto &[clusterClass, statement] : clustersToExport) {
    auto materializedResult =
        getData(clusterClass, filter.getAnalyzer(), filter.getFilter(), statement, grouping)->Cast<duckdb::StreamQueryResult>().Materialize();

    auto prepareTable = [sizeX, sizeY](size_t col, const PreparedStatement &statement, table::Table &results) {
      results.setTitle(statement.getColumnAt(col).createHeader());

      for(uint8_t row = 0; row < sizeY; row++) {
        char toWrt = row + 'A';

        results.getMutableRowHeader()[row] = std::string(1, toWrt);
        for(uint8_t col = 0; col < sizeX; col++) {
          results.getMutableColHeader()[col] = std::to_string(col + 1);
          results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, true, ""});
        }
      }
    };

    size_t columnNr = statement.getColSize();

    for(size_t row = 0; row < materializedResult->RowCount(); row++) {
      int32_t tabIdx = 0;

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
          if(!clustersToExport.containsTable(tabIdx)) {
            prepareTable(col, statement, clustersToExport.getTable(tabIdx));
          }

          double value = materializedResult->GetValue(col, row).GetValue<double>();
          if(grouping == Grouping::BY_WELL) {
            clustersToExport.setData(clusterClass, statement.getColNames(), col, tabIdx, pos.y, pos.x,
                                     table::TableCell{value, imageId, validity == 0, filename});
          } else {
            clustersToExport.setData(clusterClass, statement.getColNames(), col, tabIdx, pos.y, pos.x,
                                     table::TableCell{value, groupId, validity == 0, filename});
          }
          tabIdx++;
        }

      } catch(const duckdb::InternalException &) {
      }
    }
  }
  return clustersToExport.getResult();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerGroup::getData(const settings::ClassificatorSettingOut &clusterAndClass, db::Database *analyzer, const QueryFilter::ObjectFilter &filter,
                            const PreparedStatement &channelFilter, Grouping grouping) -> std::unique_ptr<duckdb::QueryResult>
{
  auto [sql, params] = toSQL(clusterAndClass, filter, channelFilter, grouping);
  std::cout << "-----------" << std::endl;
  std::cout << sql << std::endl;
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
auto StatsPerGroup::toSQL(const settings::ClassificatorSettingOut &clusterAndClass, const QueryFilter::ObjectFilter &filter,
                          const PreparedStatement &channelFilter, Grouping grouping) -> std::pair<std::string, DbArgs_t>
{
  std::string sql =
      "WITH innerTable AS (\n"
      "SELECT\n" +
      channelFilter.createStatsQuery(true, enums::Stats::OFF) +
      "t1.object_id,\n"
      "t1.image_id\n"
      "FROM\n"
      "	objects t1\n"
      "LEFT JOIN object_measurements t2 ON\n"
      "	t1.object_id = t2.object_id\n"
      "WHERE\n"
      " t1.cluster_id=$1 AND t1.class_id=$2\n"
      "GROUP BY\n"
      "	t1.object_id, t1.image_id\n"
      "ORDER BY\n"
      "	t1.object_id\n"
      "),\n"
      "imageGrouped as (\n"
      "SELECT\n" +
      channelFilter.createStatsQuery(false) +
      "	ANY_VALUE(images_groups.group_id) as group_id,\n"
      "	ANY_VALUE(images_groups.image_group_idx) as image_group_idx,\n"
      "	ANY_VALUE(groups.pos_on_plate_x) as pos_on_plate_x,\n"
      "	ANY_VALUE(groups.pos_on_plate_y) as pos_on_plate_y,\n"
      "	ANY_VALUE(images.file_name) as file_name,\n"
      "	ANY_VALUE(images.image_id) as image_id,\n"
      "	ANY_VALUE(images.validity) as validity\n"
      "FROM\n"
      "	innerTable\n"
      "JOIN images_groups ON\n"
      "	innerTable.image_id = images_groups.image_id\n"
      "JOIN groups on\n"
      "	images_groups.group_id = groups.group_id\n"
      "JOIN images on\n"
      "	innerTable.image_id = images.image_id\n";
  if(grouping == Grouping::BY_WELL) {
    sql += "WHERE\n";
    sql += " images_groups.group_id=$3\n";
  }
  sql +=
      "GROUP BY\n"
      "	innerTable.image_id\n"
      ")\n"
      "SELECT\n";

  if(grouping == Grouping::BY_PLATE) {
    sql += channelFilter.createStatsQuery(false, grouping == Grouping::BY_WELL ? enums::Stats::OFF : enums::Stats::AVG) +
           " ANY_VALUE(imageGrouped.group_id) as group_id,\n"
           " ANY_VALUE(imageGrouped.image_group_idx) as image_group_idx,\n"
           " ANY_VALUE(imageGrouped.pos_on_plate_x) as pos_on_plate_x,\n"
           " ANY_VALUE(imageGrouped.pos_on_plate_y) as pos_on_plate_y,\n"
           " ANY_VALUE(imageGrouped.file_name) as file_name,\n"
           " ANY_VALUE(imageGrouped.image_id) as image_id,\n"
           " ANY_VALUE(imageGrouped.validity) as validity\n";
  } else {
    sql += "*";
  }
  sql += "FROM imageGrouped\n";
  if(grouping == Grouping::BY_PLATE) {
    sql += "GROUP BY group_id\n";
    sql += "ORDER BY group_id";
  } else {
    sql += "ORDER BY file_name";
  }

  if(grouping == Grouping::BY_WELL) {
    return {
        sql,
        {static_cast<uint16_t>(clusterAndClass.clusterId), static_cast<uint16_t>(clusterAndClass.classId), static_cast<uint16_t>(filter.groupId)}};
  }
  return {sql, {static_cast<uint16_t>(clusterAndClass.clusterId), static_cast<uint16_t>(clusterAndClass.classId)}};
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
