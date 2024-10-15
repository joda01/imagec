

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
          char toWrt[2];
          toWrt[0]                           = platePosY + 'A';
          toWrt[1]                           = 0;
          results.getMutableRowHeader()[row] = std::string(toWrt) + std::to_string(platePosX);
        }

        for(size_t col = 0; col < results.getColHeaderSize(); col++) {
          double value = materializedResult->GetValue(col, row).GetValue<double>();
          results.setData(row, col, table::TableCell{value, imageId, validity == 0, ""});
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
  tables.reserve(filter.clustersToExport.size());

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

    auto headers = createHeader(channels.second);
    for(size_t row = 0; row < materializedResult->RowCount(); row++) {
      try {
        size_t offset    = headers.size();
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
            pos.x = platePosX--;
          }
          if(platePosY > 0) {
            pos.y = platePosY--;
          }
        }

        for(size_t col = 0; col < offset; col++) {
          table::Table results;
          results.setTitle(headers[col]);
          prepareTable(results);
          double value = materializedResult->GetValue(col, row).GetValue<double>();
          results.setData(pos.y, pos.x, table::TableCell{value, imageId, validity == 0, filename});
        }

      } catch(const duckdb::InternalException &) {
      }
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
auto StatsPerGroup::getData(db::Database *analyzer, const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter,
                            Grouping grouping) -> std::unique_ptr<duckdb::QueryResult>
{
  auto [sql, params]                          = toSQL(filter, channelFilter, grouping);
  std::unique_ptr<duckdb::QueryResult> result = analyzer->select(sql, params);
  return result;

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
  auto buildSelect = [&channelFilter]() {
    std::string channels;
    for(const auto &[measurment, stats] : channelFilter.second.measureChannels) {
      for(const auto stat : stats) {
        if(getType(measurment) == MeasureType::INTENSITY) {
          for(const auto [cStack, _] : channelFilter.second.crossChannelStacksC) {
            channels += "ANY_VALUE(DISTINCT CASE WHEN t2.meas_stack_c = " + std::to_string(cStack) + " THEN " + getMeasurement(measurment) +
                        " END) AS " + getMeasurement(measurment) + "_" + std::to_string(cStack) + ",\n";
          }
        } else {
          channels += "ANY_VALUE(DISTINCT " + getMeasurement(measurment) + ") as " + getMeasurement(measurment) + ",\n";
        }
      }
    }
    return channels;
  };

  auto buildStats = [&channelFilter]() {
    std::string channels;
    for(const auto &[measurment, stats] : channelFilter.second.measureChannels) {
      for(const auto stat : stats) {
        if(getType(measurment) == MeasureType::INTENSITY) {
          for(const auto [cStack, _] : channelFilter.second.crossChannelStacksC) {
            channels += getStatsString(stat) + "(" + getMeasurement(measurment) + "_" + std::to_string(cStack) + ") AS " +
                        getMeasurement(measurment) + "_" + std::to_string(cStack) + ",\n";
          }
        } else {
          channels += getStatsString(stat) + "(" + getMeasurement(measurment) + ") as " + getMeasurement(measurment) + ",\n";
        }
      }
    }
    return channels;
  };

  auto buildOutersStats = [&channelFilter](enums::Stats outerStats) {
    std::string channels;
    for(const auto &[measurment, stats] : channelFilter.second.measureChannels) {
      for(const auto _ : stats) {
        if(getType(measurment) == MeasureType::INTENSITY) {
          for(const auto [cStack, _] : channelFilter.second.crossChannelStacksC) {
            channels += getStatsString(outerStats) + "(" + getMeasurement(measurment) + "_" + std::to_string(cStack) + ") AS " +
                        getMeasurement(measurment) + "_" + std::to_string(cStack) + ",\n";
          }
        } else {
          channels += getStatsString(outerStats) + "(" + getMeasurement(measurment) + ") as " + getMeasurement(measurment) + ",\n";
        }
      }
    }
    return channels;
  };

  std::string sql =
      "WITH innerTable AS (\n"
      "SELECT\n" +
      buildSelect() +
      "t1.object_id\n"
      "FROM\n"
      "	objects t1\n"
      "JOIN object_measurements t2 ON\n"
      "	t1.object_id = t2.object_id\n"
      "WHERE\n"
      " t1.cluster_id=$1 AND t2.class_id=$2\n"
      "GROUP BY\n"
      "	t1.object_id\n"
      "ORDER BY\n"
      "	t1.object_id\n"
      "),\n"
      "imageGrouped as (\n"
      "SELECT\n" +
      buildStats() +
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
      "	groups.group_id = images_groups.group_id\n"
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
      "SELECT\n" +
      buildOutersStats(grouping == Grouping::BY_WELL ? enums::Stats::OFF : enums::Stats::AVG) +
      " ANY_VALUE(imageGroupd.group_id) as group_id,\n"
      " ANY_VALUE(imageGroupd.image_group_idx) as image_group_idx,\n"
      " ANY_VALUE(imageGroupd.pos_on_plate_x) as pos_on_plate_x,\n"
      " ANY_VALUE(imageGroupd.pos_on_plate_y) as pos_on_plate_y,\n"
      " ANY_VALUE(imageGroupd.file_name) as file_name,\n"
      " ANY_VALUE(imageGroupd.image_id) as image_id,\n"
      " ANY_VALUE(imageGroupd.validity) as validity\n"
      "FROM imageGrouped";

  return {sql,
          {static_cast<uint16_t>(channelFilter.first.clusterId), static_cast<uint16_t>(channelFilter.first.classId),
           static_cast<uint16_t>(filter.groupId)}};
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
