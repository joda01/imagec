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

#include <opencv2/core/hal/interface.h>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include "backend/enums/bigtypes.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/settings/setting.hpp"
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
    uint32_t densityMapAreaSize                      = 4096;
    std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectFilter, plateRows, plateCols, densityMapAreaSize, wellImageOrder);
  };

  struct ColumnName
  {
    std::string crossChannelName;
    std::string className;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnName, crossChannelName, className);
  };

  struct ColumnKey
  {
    joda::enums::ClassId classs;
    enums::Measurement measureChannel = enums::Measurement::NONE;
    enums::Stats stats                = enums::Stats::AVG;
    int32_t crossChannelStacksC       = -1;
    int32_t zStack                    = 0;
    int32_t tStack                    = 0;

    ColumnName names;

    bool operator<(const ColumnKey &input) const
    {
      auto toInt = [](const ColumnKey &in) {
        uint32_t classClasss = static_cast<uint32_t>(in.classs);
        auto measure         = static_cast<uint8_t>(in.measureChannel);
        auto stat            = static_cast<uint8_t>(in.stats);

        stdi::uint128_t erg =
            (static_cast<stdi::uint128_t>(classClasss) << 96) | (static_cast<stdi::uint128_t>(in.crossChannelStacksC & 0xFFFF) << 80) |
            (static_cast<stdi::uint128_t>(in.zStack) << 18) | (static_cast<stdi::uint128_t>(in.tStack) << 16) | (measure << 8) | (stat);
        return erg;
      };

      return toInt(*this) < toInt(input);
    }

    bool operator==(const ColumnKey &input) const
    {
      return classs == input.classs && static_cast<int32_t>(measureChannel) == static_cast<int32_t>(input.measureChannel) &&
             static_cast<int32_t>(stats) == static_cast<int32_t>(input.stats) && crossChannelStacksC == input.crossChannelStacksC &&
             zStack == input.zStack && tStack == input.tStack;
    }

    std::string createHeader() const
    {
      std::map<uint32_t, std::string> columnHeaders;
      std::string stacks = "{Z" + std::to_string(zStack) + "/T" + std::to_string(tStack) + "}";

      if(getType(measureChannel) == MeasureType::INTENSITY) {
        return names.className + "-" + toString(measureChannel) + "[" + enums::toString(stats) + "] " + "(C" + std::to_string(crossChannelStacksC) +
               ")" + stacks;
      }
      return names.className + "-" + toString(measureChannel) + "[" + enums::toString(stats) + "]" + stacks;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnKey, classs, measureChannel, stats, crossChannelStacksC, zStack, tStack, names);
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

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnIdx, tabIdx, colIdx);
  };

  explicit QueryFilter() = default;

  void setFilter(const ObjectFilter &filter)
  {
    this->filter = filter;
  }

  bool addColumn(const ColumnIdx &colIdx, const ColumnKey &key, const ColumnName &names)
  {
    for(const auto &[_, colKey] : columns) {
      if(colKey == key) {
        return false;
      }
    }

    if(!columns.contains(colIdx)) {
      columns.emplace(colIdx, key);
    } else {
      columns[colIdx] = key;
    }

    columns[colIdx].names = names;
    return true;
  }

  void eraseColumn(const ColumnIdx colIdx)
  {
    if(columns.contains(colIdx)) {
      columns.erase(colIdx);
      std::map<ColumnIdx, ColumnKey> newColumns;
      bool startToReduce = false;

      for(const auto &[col, _] : columns) {
        auto colNew = col;
        if(colNew.colIdx > colIdx.colIdx) {
          startToReduce = true;
        }
        if(startToReduce && colIdx.tabIdx == col.tabIdx) {
          colNew.colIdx--;
        }
        newColumns.emplace(colNew, _);
      }
      columns.clear();
      columns = newColumns;
    }
  }

  [[nodiscard]] auto getColumn(const ColumnIdx &colIdx) const -> ColumnKey
  {
    return columns.at(colIdx);
  }

  [[nodiscard]] bool containsColumn(const ColumnIdx &colIdx) const
  {
    return columns.contains(colIdx);
  }

  [[nodiscard]] auto getClassesToExport() const -> ResultingTable;

  [[nodiscard]] auto getFilter() const -> const ObjectFilter &
  {
    return filter;
  }

  [[nodiscard]] auto getColumns() const -> const std::map<ColumnIdx, ColumnKey> &
  {
    return columns;
  }

private:
  /////////////////////////////////////////////////////
  ObjectFilter filter;
  std::map<ColumnIdx, ColumnKey> columns;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(QueryFilter, columns, filter);
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

  std::string createStatsQuery(bool isOuter, bool excludeInvalid, std::optional<enums::Stats> overrideStats = std::nullopt) const;
  std::string createStatsQueryJoins() const;

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
  struct QueryKey
  {
    joda::enums::ClassId classs;
    int32_t zStack = 0;
    int32_t tStack = 0;

    bool operator<(const QueryKey &key) const
    {
      auto toUint128 = [](const QueryKey &key) -> stdi::uint128_t {
        return stdi::uint128_t(static_cast<uint64>(key.classs), static_cast<uint64>(key.zStack) << 32 | static_cast<uint64>(key.tStack));
      };

      return toUint128(*this) < toUint128(key);
    }
  };

  explicit ResultingTable(const QueryFilter *);

  void setData(const QueryKey &classsAndClass, const QueryFilter::ColumnName &colName, int32_t row, int32_t dbColIx, const std::string &rowName,
               const table::TableCell &tableCell)
  {
    if(!mClassesAndClasses.contains(classsAndClass)) {
      mClassesAndClasses.emplace(classsAndClass, PreparedStatement{colName});
    }
    const PreparedStatement &prep = mClassesAndClasses.at(classsAndClass);
    auto columnKey                = prep.getColumnAt(dbColIx);

    for(auto [itr, rangeEnd] = mTableMapping.equal_range(columnKey); itr != rangeEnd; ++itr) {
      auto &element = itr->second;
      mResultingTable.at(element.tabIdx).setRowName(row, rowName);
      mResultingTable.at(element.tabIdx).setData(row, element.colIdx, tableCell);
      mResultingTable.at(element.tabIdx).setMeta({.className = colName.className});
    }
  }

  void setData(const QueryKey &classsAndClass, const QueryFilter::ColumnName &colName, int32_t dbColIx, int32_t row, int32_t col,
               const table::TableCell &tableCell, int32_t sizeX, int32_t sizeY, const std::string &header)
  {
    if(!mClassesAndClasses.contains(classsAndClass)) {
      mClassesAndClasses.emplace(classsAndClass, PreparedStatement{colName});
    }
    const PreparedStatement &prep = mClassesAndClasses.at(classsAndClass);
    auto columnKey                = prep.getColumnAt(dbColIx);

    for(auto [itr, rangeEnd] = mTableMapping.equal_range(columnKey); itr != rangeEnd; ++itr) {
      auto &element = itr->second;
      if(!mResultingTable.contains(element.colIdx)) {
        // Prepare heatmap table
        mResultingTable[element.colIdx].setTitle(header);
        for(uint8_t row = 0; row < sizeY; row++) {
          char toWrt                                                 = row + 'A';
          mResultingTable[element.colIdx].getMutableRowHeader()[row] = std::string(1, toWrt);
          for(uint8_t col = 0; col < sizeX; col++) {
            mResultingTable[element.colIdx].getMutableColHeader()[col] = std::to_string(col + 1);
            mResultingTable[element.colIdx].setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, true, ""});
          }
        }
      }

      mResultingTable[element.colIdx].setData(row, col, tableCell);
      mResultingTable[element.colIdx].setMeta({.className = colName.className});
    }
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
    return mClassesAndClasses.begin();
  }

  auto end()
  {
    return mClassesAndClasses.end();
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
  std::map<QueryKey, PreparedStatement> mClassesAndClasses;
  std::multimap<QueryFilter::ColumnKey, QueryFilter::ColumnIdx> mTableMapping;
};

}    // namespace joda::db
