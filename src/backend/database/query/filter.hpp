///
/// \file      common.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <opencv2/core/hal/interface.h>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include "backend/enums/bigtypes.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/settings/project_settings/project_plate_setup.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/settings_types.hpp"

namespace joda::db {

class Database;
class PreparedStatement;
class ResultingTable;

using QueryResult = joda::table::Table;    // <time-stack,tab-index,table>

///
/// \class
/// \author
/// \brief
///
class PreparedStatement
{
public:
  PreparedStatement(settings::ResultsSettings::ColumnName names, const settings::ResultsSettings *filter) :
      mColNames(std::move(names)), mFilter(filter)
  {
  }
  struct JoinResults
  {
    bool containsDistance     = false;
    bool containsIntensity    = false;
    bool containsIntersection = false;
  };
  [[nodiscard]] std::string createStatsQuery(bool isOuter, bool excludeInvalid, std::string offValue = "ANY_VALUE",
                                             std::optional<enums::Stats> overrideStats = std::nullopt) const;
  std::string createStatsQueryJoins(bool isImage = false, JoinResults *results = nullptr) const;
  [[nodiscard]] std::tuple<std::string, std::string> createIntersectionQuery() const;

  void addColumn(settings::ResultsSettings::ColumnKey col)
  {
    for(const auto &[_, element] : columns) {
      if(element == col) {
        return;
      }
    }

    uint32_t pos = static_cast<uint32_t>(columns.size());
    columns.emplace(pos, col);
  }

  [[nodiscard]] auto getColumns() const -> const std::map<uint32_t, settings::ResultsSettings::ColumnKey> &
  {
    return columns;
  }

  [[nodiscard]] auto getColumnAt(uint32_t dbColIdx) const -> const settings::ResultsSettings::ColumnKey &
  {
    if(!columns.contains(dbColIdx)) {
      throw std::invalid_argument("Colum >" + std::to_string(dbColIdx) + "< unknown!");
    }
    return columns.at(dbColIdx);
  }

  [[nodiscard]] auto getColSize() const
  {
    return columns.size();
  }

  [[nodiscard]] auto getColNames() const -> const settings::ResultsSettings::ColumnName &
  {
    return mColNames;
  }

private:
  /////////////////////////////////////////////////////
  static std::string getMeasurement(enums::Measurement measure, bool textual);
  static std::string getStatsString(enums::Stats stats, const std::string &offValue = "ANY_VALUE");

  /////////////////////////////////////////////////////
  std::map<uint32_t, settings::ResultsSettings::ColumnKey> columns;
  settings::ResultsSettings::ColumnName mColNames;
  const settings::ResultsSettings *mFilter;
};

///
/// \class
/// \author
/// \brief
///
class ResultingTable
{
public:
  ///
  /// \struct     QueryKey
  /// \author     Joachim Danmayr
  /// \brief      Defines which queries can be done in one statement
  ///             For each class a separate statement must be created
  ///
  struct QueryKey
  {
    joda::enums::ClassId classs;
    int32_t zStack                       = 0;
    int32_t tStack                       = 0;
    joda::enums::ClassId distanceToClass = joda::enums::ClassId::NONE;

    bool operator<(const QueryKey &key) const
    {
      auto toUint128 = [](const QueryKey &keyIn) -> stdi::uint128_t {
        if(keyIn.distanceToClass == joda::enums::ClassId::NONE) {
          // We want the none to be first this is neccessary for toHeatmap to keep the order the rows are queried
          return {static_cast<uint64>(keyIn.classs), static_cast<uint64>(keyIn.zStack) << 32 | static_cast<uint64>(keyIn.tStack)};
        }
        return {(static_cast<uint64>(keyIn.distanceToClass) + 1) << 16 | static_cast<uint64>(keyIn.classs),
                static_cast<uint64>(keyIn.zStack) << 32 | static_cast<uint64>(keyIn.tStack)};
      };

      return toUint128(*this) < toUint128(key);
    }
  };

  explicit ResultingTable(const settings::ResultsSettings *);

  void setData(const QueryKey &classsAndClass, const settings::ResultsSettings::ColumnName &colName, uint32_t row, uint32_t dbColIx,
               const std::string & /*rowName*/, const table::TableCell &tableCell)
  {
    if(!mClassesAndClasses.contains(classsAndClass)) {
      mClassesAndClasses.emplace(classsAndClass, PreparedStatement{colName, mFilter});
    }
    const PreparedStatement &prep = mClassesAndClasses.at(classsAndClass);
    auto columnKey                = prep.getColumnAt(dbColIx);

    for(auto [itr, rangeEnd] = mTableMapping.equal_range(columnKey); itr != rangeEnd; ++itr) {
      auto &element = itr->second;
      mResultingTable.setData(row, element.colIdx, tableCell);
    }
  }

  [[nodiscard]] int32_t getColIdxFromDbColIdx(const PreparedStatement &statement, uint32_t dbColIdx) const
  {
    int32_t colIdx        = 0;
    const auto &columnKey = statement.getColumnAt(dbColIdx);
    for(auto [itr, rangeEnd] = mTableMapping.equal_range(columnKey); itr != rangeEnd; ++itr) {
      const auto &element = itr->second;
      colIdx              = element.colIdx;
    }
    return colIdx;
  }

  void setRowID(const QueryKey &classsAndClass, const settings::ResultsSettings::ColumnName &colName, uint32_t row, const std::string &rowName,
                uint64_t rowId)
  {
    if(!mClassesAndClasses.contains(classsAndClass)) {
      mClassesAndClasses.emplace(classsAndClass, PreparedStatement{colName, mFilter});
    }

    for(auto [itr, element] : mTableMapping) {
      mResultingTable.setDataId(row, element.colIdx, rowId, rowName);
    }
  }

  auto getTable() -> table::Table &
  {
    return mResultingTable;
  }

  auto begin()
  {
    return mClassesAndClasses.begin();
  }

  auto end()
  {
    return mClassesAndClasses.end();
  }

  auto getResult() -> const QueryResult &
  {
    return mResultingTable;
  }

  auto mutableResult() -> QueryResult &
  {
    return mResultingTable;
  }

  void clearTables()
  {
    mResultingTable.clear();
  }

private:
  /////////////////////////////////////////////////////
  QueryResult mResultingTable;
  std::map<QueryKey, PreparedStatement> mClassesAndClasses;
  std::multimap<settings::ResultsSettings::ColumnKey, settings::ResultsSettings::ColumnIdx> mTableMapping;
  const settings::ResultsSettings *mFilter;
};

}    // namespace joda::db
