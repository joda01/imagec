#pragma once

#include <cstdint>
#include "../database.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

class StatsPerGroup
{
public:
  enum class Grouping
  {
    BY_WELL,
    BY_PLATE,
    BY_IMAGE
  };

  static auto toTable(const QueryFilter &filter, Grouping grouping) -> QueryResult;
  static auto toHeatmap(const QueryFilter &filter, Grouping grouping) -> QueryResult;
  static auto toSQL(const settings::ClassificatorSettingOut &clusterAndClass, const QueryFilter::ObjectFilter &filter,
                    const PreparedStatement &channelFilter, Grouping grouping) -> std::pair<std::string, DbArgs_t>;

private:
  static auto getData(const settings::ClassificatorSettingOut &clusterAndClass, db::Database *analyzer, const QueryFilter::ObjectFilter &filter,
                      const PreparedStatement &channelFilter, Grouping grouping) -> std::unique_ptr<duckdb::QueryResult>;
};
}    // namespace joda::db
