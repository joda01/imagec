#pragma once

#include <cstdint>
#include "../database.hpp"
#include "backend/helper/database/plugins/helper.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

class StatsPerGroup
{
public:
  enum class Grouping
  {
    BY_WELL,
    BY_PLATE
  };

  static auto toTable(const QueryFilter &filter, Grouping grouping) -> std::vector<joda::table::Table>;
  static auto toHeatmap(const QueryFilter &filter, Grouping grouping) -> std::vector<joda::table::Table>;
  static auto toSQL(const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter, Grouping grouping)
      -> std::pair<std::string, DbArgs_t>;

private:
  static auto getData(db::Database *analyzer, const QueryFilter::ObjectFilter &filter, const QueryFilter::ChannelFilter &channelFilter,
                      Grouping grouping) -> std::unique_ptr<duckdb::QueryResult>;
};
}    // namespace joda::db
