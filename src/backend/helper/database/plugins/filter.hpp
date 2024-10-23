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

#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
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

    ColumnName names;

    bool operator<(const ColumnKey &input) const
    {
      auto toInt = [](const ColumnKey &in) {
        uint32_t classCluster = in.clusterClass.toUint32(in.clusterClass.clusterId, in.clusterClass.classId);
        auto measure          = static_cast<uint8_t>(in.measureChannel);
        auto stat             = static_cast<uint8_t>(in.stats);

        __uint128_t erg = (static_cast<__uint128_t>(classCluster) << 96) | (static_cast<__uint128_t>(in.crossChannelStacksC & 0xFFFF) << 80) |
                          (static_cast<__uint128_t>(in.zStack) << 18) | (static_cast<__uint128_t>(in.tStack) << 16) | (measure << 8) | (stat);
        return erg;
      };

      return toInt(*this) < toInt(input);
    }

    bool operator==(const ColumnKey &input) const
    {
      return clusterClass == input.clusterClass && static_cast<int32_t>(measureChannel) == static_cast<int32_t>(input.measureChannel) &&
             static_cast<int32_t>(stats) == static_cast<int32_t>(input.stats) && crossChannelStacksC == input.crossChannelStacksC &&
             zStack == input.zStack && tStack == input.tStack;
    }

    std::string createHeader() const
    {
      std::map<uint32_t, std::string> columnHeaders;
      if(getType(measureChannel) == MeasureType::INTENSITY) {
        return names.clusterName + "@" + names.className + "-" + toString(measureChannel) + "[" + enums::toString(stats) + "] " + "(CH" +
               std::to_string(crossChannelStacksC) + ")";
      }
      return names.clusterName + "@" + names.className + "-" + toString(measureChannel) + "[" + enums::toString(stats) + "]";
    }
  };

  struct ColumnIdx
  {
    int32_t tabIdx = 0;
    int32_t colIdx = 0;
    bool operator<(const ColumnIdx &input) const
    {
      auto toInt = [](const ColumnIdx &in) -> uint64_t {
        uint64_t erg = (static_cast<uint64_t>(in.tabIdx) << 32) | (static_cast<uint64_t>(in.colIdx));
        return erg;
      };
      return toInt(*this) < toInt(input);
    }
  };

  explicit QueryFilter() = default;

  void setFilter(db::Database *db, const ObjectFilter &filter)
  {
    mAnalyzer = db;
    mFilter   = filter;
  }

  bool addColumn(const ColumnIdx &colIdx, const ColumnKey &key, const ColumnName &names)
  {
    for(const auto &[_, colKey] : mColumns) {
      if(colKey == key) {
        return false;
      }
    }

    if(!mColumns.contains(colIdx)) {
      mColumns.emplace(colIdx, key);
    } else {
      mColumns[colIdx] = key;
    }

    mColumns[colIdx].names = names;
    return true;
  }

  void eraseColumn(const ColumnIdx colIdx)
  {
    if(mColumns.contains(colIdx)) {
      mColumns.erase(colIdx);
      std::map<ColumnIdx, ColumnKey> newColumns;
      bool startToReduce = false;

      for(const auto &[col, _] : mColumns) {
        auto colNew = col;
        if(colNew.colIdx > colIdx.colIdx) {
          startToReduce = true;
        }
        if(startToReduce && colIdx.tabIdx == col.tabIdx) {
          colNew.colIdx--;
        }
        newColumns.emplace(colNew, _);
      }
      mColumns.clear();
      mColumns = newColumns;
    }
  }

  [[nodiscard]] auto getColumn(const ColumnIdx &colIdx) const -> ColumnKey
  {
    return mColumns.at(colIdx);
  }

  [[nodiscard]] bool containsColumn(const ColumnIdx &colIdx) const
  {
    return mColumns.contains(colIdx);
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

private:
  /////////////////////////////////////////////////////
  db::Database *mAnalyzer = nullptr;
  ObjectFilter mFilter;
  std::map<ColumnIdx, ColumnKey> mColumns;
};

///
/// \class
/// \author
/// \brief
///
class PreparedStatement
{
public:
  PreparedStatement(QueryFilter::ColumnName names) : mColNames(std::move(names))
  {
  }

  std::string createStatsQuery(bool isDistinct, std::optional<enums::Stats> overrideStats = std::nullopt) const;

  void addColumn(QueryFilter::ColumnKey col)
  {
    for(const auto &[_, element] : columns) {
      if(element == col) {
        return;
      }
    }

    size_t pos = columns.size();
    columns.emplace(pos, col);
  }

  [[nodiscard]] auto getColumns() const -> const std::map<int32_t, QueryFilter::ColumnKey> &
  {
    return columns;
  }

  [[nodiscard]] auto getColumnAt(int32_t dbColIdx) const -> const QueryFilter::ColumnKey &
  {
    if(!columns.contains(dbColIdx)) {
      throw std::invalid_argument("Colum unknown!");
    }
    return columns.at(dbColIdx);
  }

  auto getColSize() const
  {
    return columns.size();
  }

  [[nodiscard]] auto getColNames() const -> const QueryFilter::ColumnName &
  {
    return mColNames;
  }

private:
  /////////////////////////////////////////////////////
  static std::string getMeasurement(enums::Measurement measure, bool textual);
  static std::string getStatsString(enums::Stats stats);

  /////////////////////////////////////////////////////
  std::map<int32_t, QueryFilter::ColumnKey> columns;
  QueryFilter::ColumnName mColNames;
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

  void setData(const settings::ClassificatorSettingOut &clusterAndClass, const QueryFilter::ColumnName &colName, int32_t row, int32_t dbColIx,
               const std::string &rowName, const table::TableCell &tableCell)
  {
    if(!mClustersAndClasses.contains(clusterAndClass)) {
      mClustersAndClasses.emplace(clusterAndClass, PreparedStatement{colName});
    }
    const PreparedStatement &prep = mClustersAndClasses.at(clusterAndClass);
    auto columnKey                = prep.getColumnAt(dbColIx);

    for(auto [itr, rangeEnd] = mTableMapping.equal_range(columnKey); itr != rangeEnd; ++itr) {
      auto &element = itr->second;
      mResultingTable.at(element.tabIdx).setRowName(row, rowName);
      mResultingTable.at(element.tabIdx).setData(row, element.colIdx, tableCell);
      mResultingTable.at(element.tabIdx).setMeta({.clusterName = colName.clusterName, .className = colName.className});
    }
  }

  void setData(const settings::ClassificatorSettingOut &clusterAndClass, const QueryFilter::ColumnName &colName, int32_t dbColIx, int32_t tabIndex,
               int32_t row, int32_t col, const table::TableCell &tableCell)
  {
    if(!mClustersAndClasses.contains(clusterAndClass)) {
      mClustersAndClasses.emplace(clusterAndClass, PreparedStatement{colName});
    }
    const PreparedStatement &prep = mClustersAndClasses.at(clusterAndClass);
    auto columnKey                = prep.getColumnAt(dbColIx);
    mResultingTable[tabIndex].setData(row, col, tableCell);
    mResultingTable[tabIndex].setMeta({.clusterName = colName.clusterName, .className = colName.className});
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
  std::map<int32_t, table::Table> mResultingTable;
  std::map<settings::ClassificatorSettingOut, PreparedStatement> mClustersAndClasses;
  std::multimap<QueryFilter::ColumnKey, QueryFilter::ColumnIdx> mTableMapping;
};

}    // namespace joda::db
