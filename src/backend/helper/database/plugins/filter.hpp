///
/// \file      common.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/settings/settings_types.hpp"

namespace joda::db {

class Database;
class PreparedStatement;
class ResultingTable;

using QueryResult = std::map<int32_t, joda::table::Table>;

enum MeasureType
{
  OBJECT,
  INTENSITY
};

inline MeasureType getType(enums::Measurement measure)
{
  switch(measure) {
    case enums::Measurement::INTENSITY_SUM:
    case enums::Measurement::INTENSITY_AVG:
    case enums::Measurement::INTENSITY_MIN:
    case enums::Measurement::INTENSITY_MAX:
      return MeasureType::INTENSITY;
    case enums::Measurement::ORIGIN_OBJECT_ID:
    case enums::Measurement::CENTER_OF_MASS_X:
    case enums::Measurement::CENTER_OF_MASS_Y:
    case enums::Measurement::CONFIDENCE:
    case enums::Measurement::AREA_SIZE:
    case enums::Measurement::PERIMETER:
    case enums::Measurement::CIRCULARITY:
    case enums::Measurement::COUNT:
    case enums::Measurement::BOUNDING_BOX_WIDTH:
    case enums::Measurement::BOUNDING_BOX_HEIGHT:
      return MeasureType::OBJECT;
  }
  return MeasureType::OBJECT;
}

class QueryFilter
{
public:
  struct ObjectFilter
  {
    uint8_t plateId                                  = 0;
    uint16_t groupId                                 = 0;
    uint64_t imageId                                 = 0;
    uint16_t plateRows                               = 0;
    uint16_t plateCols                               = 0;
    uint32_t heatmapAreaSize                         = 200;
    std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
  };

  struct ColumnName
  {
    std::string crossChannelName;
    std::string clusterName;
    std::string className;
  };

  struct ColumnKey
  {
    settings::ClassificatorSettingOut clusterClass;
    enums::Measurement measureChannel = enums::Measurement::NONE;
    enums::Stats stats                = enums::Stats::AVG;
    int32_t crossChannelStacksC       = -1;
    int32_t zStack                    = 0;
    int32_t tStack                    = 0;

    bool operator<(const ColumnKey &input) const
    {
      return clusterClass < input.clusterClass || static_cast<int32_t>(measureChannel) < static_cast<int32_t>(input.measureChannel) ||
             static_cast<int32_t>(stats) < static_cast<int32_t>(input.stats) || crossChannelStacksC < input.crossChannelStacksC;
    }

    bool operator==(const ColumnKey &input) const
    {
      return static_cast<int32_t>(measureChannel) == static_cast<int32_t>(input.measureChannel) &&
             static_cast<int32_t>(stats) == static_cast<int32_t>(input.stats) && crossChannelStacksC == input.crossChannelStacksC &&
             zStack == input.zStack && tStack == input.tStack;
    }

    std::string createHeader(const QueryFilter *filter) const
    {
      if(!filter->mColumnNames.contains(*this)) {
        return "-";
      }
      auto names = filter->mColumnNames.at(*this);
      std::map<uint32_t, std::string> columnHeaders;
      if(getType(measureChannel) == MeasureType::INTENSITY) {
        return names.className + "-" + toString(measureChannel) + "[" + enums::toString(stats) + "] " + "(CH" + std::to_string(crossChannelStacksC) +
               ")";
      }
      return names.className + "-" + toString(measureChannel) + "[" + enums::toString(stats) + "]";
    }
  };

  struct ColumnIdx
  {
    int32_t tabIdx = 0;
    int32_t colIdx = 0;
    bool operator<(const ColumnIdx &input) const
    {
      return tabIdx < input.tabIdx || colIdx < input.colIdx;
    }
  };

  explicit QueryFilter() = default;

  void setFilter(db::Database *db, const ObjectFilter &filter)
  {
    mAnalyzer = db;
    mFilter   = filter;
  }

  void addColumn(const ColumnIdx &colIdx, const ColumnKey &key, const ColumnName &names)
  {
    if(!mColumns.contains(colIdx)) {
      mColumns.emplace(colIdx, key);
    } else {
      mColumns[colIdx] = key;
    }

    mColumnNames[key] = names;
  }

  [[nodiscard]] auto getClustersAndClassesToExport() const -> ResultingTable;

  [[nodiscard]] auto getAnalyzer() const -> db::Database *
  {
    return mAnalyzer;
  }

  [[nodiscard]] auto getFilter() const -> const ObjectFilter &
  {
    return mFilter;
  }

  [[nodiscard]] auto getColumns() const -> const std::map<ColumnIdx, ColumnKey> &
  {
    return mColumns;
  }

  [[nodiscard]] auto getColumnName(const ColumnKey &key) const -> const ColumnName &
  {
    return mColumnNames.at(key);
  }

private:
  /////////////////////////////////////////////////////
  db::Database *mAnalyzer = nullptr;
  ObjectFilter mFilter;
  std::map<ColumnIdx, ColumnKey> mColumns;
  std::map<ColumnKey, ColumnName> mColumnNames;
};

///
/// \class
/// \author
/// \brief
///
class PreparedStatement
{
public:
  std::string createStatsQuery(bool isDistinct, std::optional<enums::Stats> overrideStats = std::nullopt) const;

  void addColumn(int32_t colIdx, QueryFilter::ColumnKey col)
  {
    for(const auto &[_, element] : columns) {
      if(element == col) {
        return;
      }
    }

    columns.emplace(colIdx, col);
  }

  [[nodiscard]] auto getColumns() const -> const std::map<int32_t, QueryFilter::ColumnKey> &
  {
    return columns;
  }

  [[nodiscard]] auto getColumnAt(int32_t dbColIdx) const -> const QueryFilter::ColumnKey &
  {
    if(dbColIdx >= columns.size()) {
      throw std::invalid_argument("Colum unknown!");
    }
    return columns.at(dbColIdx);
  }

  auto getColSize() const
  {
    return columns.size();
  }

private:
  /////////////////////////////////////////////////////
  static std::string getMeasurement(enums::Measurement measure, bool textual);
  static std::string getStatsString(enums::Stats stats);

  /////////////////////////////////////////////////////
  std::map<int32_t, QueryFilter::ColumnKey> columns;
};

///
/// \class
/// \author
/// \brief
///
class ResultingTable
{
public:
  explicit ResultingTable(const QueryFilter *);

  void setData(const settings::ClassificatorSettingOut &clusterAndClass, int32_t row, int32_t dbColIx, const table::TableCell &tableCell)
  {
    const PreparedStatement &prep = mClustersAndClasses[clusterAndClass];
    auto columnKey                = prep.getColumnAt(dbColIx);

    auto tableColumnsToWriteTo = mTableMapping.equal_range(columnKey);
    for(auto it = tableColumnsToWriteTo.first; it != tableColumnsToWriteTo.second; ++it) {
      auto &element = it->second;
      mResultingTable.at(element.tabIdx).setData(row, element.colIdx, tableCell);
    }
  }

  void setData(const settings::ClassificatorSettingOut &clusterAndClass, int32_t dbColIx, int32_t tabIndex, int32_t row, int32_t col,
               const table::TableCell &tableCell)
  {
    const PreparedStatement &prep = mClustersAndClasses[clusterAndClass];
    auto columnKey                = prep.getColumnAt(dbColIx);
    auto colName                  = mFilter->getColumnName(columnKey);
    mResultingTable[tabIndex].setData(row, col, tableCell);
  }

  auto getTable(int32_t tabIdx) -> table::Table &
  {
    return mResultingTable[tabIdx];
  }

  auto containsTable(int32_t tabIdx) -> bool
  {
    return mResultingTable.contains(tabIdx);
  }

  auto begin()
  {
    return mClustersAndClasses.begin();
  }

  auto end()
  {
    return mClustersAndClasses.end();
  }

  auto getResult() -> const std::map<int32_t, table::Table> &
  {
    return mResultingTable;
  }

  void clearTables()
  {
    mResultingTable.clear();
  }

private:
  /////////////////////////////////////////////////////
  const QueryFilter *mFilter;
  std::map<int32_t, table::Table> mResultingTable;
  std::map<settings::ClassificatorSettingOut, PreparedStatement> mClustersAndClasses;
  std::multimap<QueryFilter::ColumnKey, QueryFilter::ColumnIdx> mTableMapping;
};

}    // namespace joda::db
