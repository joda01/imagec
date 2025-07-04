#pragma once

#include <cstdint>
#include "../database.hpp"
#include "backend/database/plugins/filter.hpp"
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

  static auto toTable(db::Database *database, const settings::ResultsSettings &filter, Grouping grouping,
                      settings::ResultsSettings *resultingFilter = nullptr) -> QueryResult;
  static auto toSQL(const db::ResultingTable::QueryKey &classsAndClass, const settings::ResultsSettings::ObjectFilter &filter,
                    const PreparedStatement &channelFilter, Grouping grouping) -> std::pair<std::string, DbArgs_t>;

private:
  static auto getData(const db::ResultingTable::QueryKey &classsAndClass, db::Database *analyzer,
                      const settings::ResultsSettings::ObjectFilter &filter, const PreparedStatement &channelFilter, Grouping grouping)
      -> std::unique_ptr<duckdb::QueryResult>;
};
}    // namespace joda::db
