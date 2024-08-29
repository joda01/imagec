
#pragma once

#include <cstdint>
#include <string>
#include "../database.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/plugins/helper.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

class StatsPerPlate
{
public:
  static auto toTable(const QueryFilter &filter) -> joda::table::Table;
  static auto toHeatmap(const QueryFilter &filter) -> joda::table::Table;

private:
  static auto getData(const QueryFilter &filter) -> std::unique_ptr<duckdb::QueryResult>;
};
}    // namespace joda::db
