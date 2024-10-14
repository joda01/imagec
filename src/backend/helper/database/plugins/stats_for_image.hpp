#pragma once

#include <cstdint>
#include <string>
#include "../database.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/plugins/helper.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

class StatsPerImage
{
public:
  static auto toTable(const QueryFilter &filter) -> std::vector<joda::table::Table>;
  static auto toHeatmap(const QueryFilter &filter) -> std::vector<joda::table::Table>;
  static auto toHeatmapList(const QueryFilter &filter) -> std::vector<joda::table::Table>;
  static auto toSqlTable(const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter)
      -> std::pair<std::string, DbArgs_t>;
  static auto toSqlHeatmap(const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter)
      -> std::pair<std::string, DbArgs_t>;

private:
  struct ImgInfo
  {
    uint64_t width;
    uint64_t height;
    std::string controlImgPath;
  };

  static auto densityMap(db::Database *analyzer, const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter)
      -> std::tuple<std::unique_ptr<duckdb::QueryResult>, ImgInfo>;
};
}    // namespace joda::db
