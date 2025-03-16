#pragma once

#include <cstdint>
#include <string>
#include "../database.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db {

class StatsPerImage
{
public:
  static auto toTable(db::Database *database, const settings::ResultsSettings &filter) -> QueryResult;
  static auto toHeatmap(db::Database *database, const settings::ResultsSettings &filter) -> QueryResult;
  static auto toSqlTable(const db::ResultingTable::QueryKey &classsAndClass, const settings::ResultsSettings::ObjectFilter &filter,
                         const PreparedStatement &channelFilter) -> std::pair<std::string, DbArgs_t>;
  static auto toSqlHeatmap(const db::ResultingTable::QueryKey &classsAndClass, const settings::ResultsSettings::ObjectFilter &filter,
                           const settings::DensityMapSettings &densityMapSettings, const PreparedStatement &channelFilter)
      -> std::pair<std::string, DbArgs_t>;

private:
  struct ImgInfo
  {
    uint64_t width;
    uint64_t height;
    std::string controlImgPath;
  };

  static auto densityMap(const db::ResultingTable::QueryKey &classsAndClass, db::Database *analyzer,
                         const settings::ResultsSettings::ObjectFilter &filter, const settings::DensityMapSettings &densityMapSettings,
                         const PreparedStatement &channelFilter) -> std::tuple<std::unique_ptr<duckdb::QueryResult>, ImgInfo>;
};
}    // namespace joda::db
