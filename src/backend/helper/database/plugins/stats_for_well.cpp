

#include "stats_for_well.hpp"
#include <cstddef>
#include <string>
#include "backend/enums/enum_measurements.hpp"

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
auto StatsPerGroup::toTable(const QueryFilter &filter, Grouping grouping) -> std::vector<joda::table::Table>
{
  std::vector<joda::table::Table> tables;
  tables.reserve(filter.clustersToExport.size());

  for(const auto &channels : filter.clustersToExport) {
    table::Table results;
    results.setTitle(channels.second.first.clusterName);
    results.setMeta({channels.second.first.clusterName, channels.second.first.className});
    results.setColHeader(createHeader(channels.second));
    auto materializedResult = getData(filter.analyzer, filter.filter, channels, grouping)->Cast<duckdb::StreamQueryResult>().Materialize();

    for(size_t row = 0; row < materializedResult->RowCount(); row++) {
      try {
        size_t offset    = results.getColHeaderSize();
        auto groupId     = materializedResult->GetValue(offset + 0, row).GetValue<uint16_t>();
        auto imgGroupIdx = materializedResult->GetValue(offset + 1, row).GetValue<uint32_t>();
        auto platePosX   = materializedResult->GetValue(offset + 2, row).GetValue<uint32_t>();
        auto platePosY   = materializedResult->GetValue(offset + 3, row).GetValue<uint32_t>();
        auto filename    = materializedResult->GetValue(offset + 4, row).GetValue<std::string>();
        auto imageId     = materializedResult->GetValue(offset + 5, row).GetValue<uint64_t>();
        auto validity    = materializedResult->GetValue(offset + 6, row).GetValue<uint64_t>();

        if(grouping == Grouping::BY_WELL) {
          results.getMutableRowHeader()[row] = filename;
        } else {
          char toWrt                         = (platePosY - 1) + 'A';
          results.getMutableRowHeader()[row] = std::string(1, toWrt) + std::to_string(platePosX);
        }

        for(size_t col = 0; col < results.getColHeaderSize(); col++) {
          double value = materializedResult->GetValue(col, row).GetValue<double>();

          if(grouping == Grouping::BY_WELL) {
            results.setData(row, col, table::TableCell{value, imageId, validity == 0, ""});
          } else {
            results.setData(row, col, table::TableCell{value, groupId, validity == 0, ""});
          }
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
auto StatsPerGroup::toHeatmap(const QueryFilter &filter, Grouping grouping) -> std::vector<joda::table::Table>
{
  std::vector<joda::table::Table> tables;

  for(const auto &channels : filter.clustersToExport) {
    auto materializedResult = getData(filter.analyzer, filter.filter, channels, grouping)->Cast<duckdb::StreamQueryResult>().Materialize();

    int32_t sizeX = 0;
    int32_t sizeY = 0;
    std::map<int32_t, ImgPositionInWell> wellPos;
    if(grouping == Grouping::BY_WELL) {
      wellPos = transformMatrix(filter.filter.wellImageOrder, sizeX, sizeY);
    } else {
      sizeX = filter.filter.plateCols;
      sizeY = filter.filter.plateRows;
    }

    auto prepareTable = [&channels, sizeX, sizeY](table::Table &results) {
      for(uint8_t row = 0; row < sizeY; row++) {
        char toWrt[2];
        toWrt[0]                           = row + 'A';
        toWrt[1]                           = 0;
        results.getMutableRowHeader()[row] = std::string(toWrt);
        for(uint8_t col = 0; col < sizeX; col++) {
          results.getMutableColHeader()[col] = std::to_string(col + 1);
          results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, true, ""});
        }
      }
    };

    auto headers  = createHeader(channels.second);
    size_t offset = headers.size();
    std::vector<joda::table::Table> innerTables;

    for(size_t row = 0; row < materializedResult->RowCount(); row++) {
      try {
        auto groupId     = materializedResult->GetValue(offset + 0, row).GetValue<uint16_t>();
        auto imgGroupIdx = materializedResult->GetValue(offset + 1, row).GetValue<uint32_t>();
        auto platePosX   = materializedResult->GetValue(offset + 2, row).GetValue<uint32_t>();
        auto platePosY   = materializedResult->GetValue(offset + 3, row).GetValue<uint32_t>();
        auto filename    = materializedResult->GetValue(offset + 4, row).GetValue<std::string>();
        auto imageId     = materializedResult->GetValue(offset + 5, row).GetValue<uint64_t>();
        auto validity    = materializedResult->GetValue(offset + 6, row).GetValue<uint64_t>();
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

        for(size_t col = 0; col < offset; col++) {
          if(innerTables.size() <= col) {
            auto &results = innerTables.emplace_back();
            prepareTable(results);
          }
          joda::table::Table &results = innerTables.at(col);

          results.setTitle(headers[col]);
          results.setMeta({channels.second.first.clusterName, channels.second.first.className});

          double value = materializedResult->GetValue(col, row).GetValue<double>();
          if(grouping == Grouping::BY_WELL) {
            results.setData(pos.y, pos.x, table::TableCell{value, imageId, validity == 0, filename});
          } else {
            results.setData(pos.y, pos.x, table::TableCell{value, groupId, validity == 0, filename});
          }
        }

      } catch(const duckdb::InternalException &) {
      }
    }

    tables.insert(tables.end(), innerTables.begin(), innerTables.end());
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
auto StatsPerGroup::getData(db::Database *analyzer, const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter,
                            Grouping grouping) -> std::unique_ptr<duckdb::QueryResult>
{
  auto [sql, params] = toSQL(filter, channelFilter, grouping);
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
auto StatsPerGroup::toSQL(const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter, Grouping grouping)
    -> std::pair<std::string, DbArgs_t>
{
  std::string sql =
      "WITH innerTable AS (\n"
      "SELECT\n" +
      createStats(channelFilter.second, true, enums::Stats::OFF) +
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
      createStats(channelFilter.second, false) +
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
    sql += createStats(channelFilter.second, false, grouping == Grouping::BY_WELL ? enums::Stats::OFF : enums::Stats::AVG) +
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
    return {sql,
            {static_cast<uint16_t>(channelFilter.first.clusterId), static_cast<uint16_t>(channelFilter.first.classId),
             static_cast<uint16_t>(filter.groupId)}};
  }
  return {sql, {static_cast<uint16_t>(channelFilter.first.clusterId), static_cast<uint16_t>(channelFilter.first.classId)}};
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
