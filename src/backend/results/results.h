///
/// \file      results.h
/// \author    Joachim Danmayr
/// \date      2023-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include "../image_processing/detection/detection_response.hpp"
#include "xlsxwriter.h"

namespace joda::results {

#define CSV_SEPARATOR ","

///
/// \class      Statistics
/// \author     Joachim Danmayr
/// \brief      Statistics class which calculates
///             Number, Sum, Min, Max and Square Sum
///
class Statistics
{
public:
  /////////////////////////////////////////////////////
  static constexpr int NR_OF_VALUE = 6;
  Statistics()                     = default;
  ~Statistics()                    = default;
  /// \todo mark copy constructor as deleted
  Statistics(const Statistics &other) :
      mNr(other.mNr), mInvalid(other.mInvalid), mSum(other.mSum), mMin(other.mMin), mMax(other.mMax), mMean(other.mMean)
  {
  }

  Statistics &operator=(const Statistics other)
  {
    mNr      = other.mNr;
    mInvalid = other.mInvalid;
    mSum     = other.mSum;
    mMin     = other.mMin;
    mMax     = other.mMax;
    mMean    = other.mMean;

    return *this;
  }

  /////////////////////////////////////////////////////
  void addValue(double val)
  {
    std::lock_guard<std::mutex> lock(mAddMutex);
    if(mNr == 0) {
      mMin = val;
      mMax = val;
    } else {
      mMin = std::min(mMin, val);
      mMax = std::max(mMax, val);
    }
    mNr++;

    mSum += val;
    mMean = mSum / static_cast<double>(mNr);
  }

  void incrementInvalid()
  {
    std::lock_guard<std::mutex> lock(mAddMutex);
    mInvalid++;
  }

  void reset()
  {
    mNr   = 0;
    mSum  = 0;
    mMin  = 0;
    mMax  = 0;
    mMean = 0;
  }

  [[nodiscard]] uint64_t getNr() const
  {
    return mNr;
  }

  [[nodiscard]] uint64_t getInvalid() const
  {
    return mInvalid;
  }

  [[nodiscard]] double getSum() const
  {
    return mSum;
  }

  [[nodiscard]] double getMin() const
  {
    return mMin;
  }

  [[nodiscard]] double getMax() const
  {
    return mMax;
  }

  [[nodiscard]] double getAvg() const
  {
    return mMean;
  }

  static auto getStatisticsTitle() -> const std::array<std::string, NR_OF_VALUE>;
  [[nodiscard]] auto getStatistics() const -> const std::array<double, NR_OF_VALUE>;

private:
  /////////////////////////////////////////////////////
  uint64_t mNr      = 0;
  uint64_t mInvalid = 0;
  double mSum       = 0;
  double mMin       = 0;
  double mMax       = 0;
  double mMean      = 0;
  std::mutex mAddMutex;
};

///
/// \class      Table
/// \author     Joachim Danmayr
/// \brief      A table representation
///             Consisting of columns and rows, werby
///             for each column a statistics is calculated
///
///             |Col 0 |Col 1
///     --------|------|--------
///       Row 0 |      |
///       Row 1 |      |
///       Row 2 |      |
///     --------|------|--------
///   Statitics |      |
///
///
///
class Table
{
public:
  /////////////////////////////////////////////////////

  struct Row
  {
    std::variant<double, joda::func::ParticleValidity> value;
    joda::func::ParticleValidity validity;
  };

  struct TableMeta
  {
    std::string tableName;
    joda::func::ResponseDataValidity validity = joda::func::ResponseDataValidity::VALID;
  };

  /////////////////////////////////////////////////////
  using Row_t       = std::map<uint32_t, Row>;      // Row index
  using Table_t     = std::map<uint64_t, Row_t>;    // Column index
  using ColumnKey_t = int64_t;

  /////////////////////////////////////////////////////
  void setTableName(const std::string &name);
  void setTableValidity(joda::func::ResponseDataValidity valid);
  auto getTableValidity() const -> joda::func::ResponseDataValidity;

  void setColumnName(uint64_t idx, const std::string &colName, ColumnKey_t key);
  const std::string &getTableName() const;
  auto getColumnNameAt(uint64_t colIdx) const -> const std::string;
  auto getColumnKeyAt(uint64_t colIdx) const -> ColumnKey_t;
  auto getRowNameAt(uint64_t rowIdx) const -> const std::string;
  void setRowName(uint64_t rowIdx, const std::string &);
  auto getRowNames() const -> const std::map<uint64_t, std::string> &;
  auto appendValueToColumn(uint64_t colIdx, double value, joda::func::ParticleValidity) -> int64_t;
  auto appendValueToColumn(const std::string &rowName, uint64_t colIdx, double value, joda::func::ParticleValidity)
      -> int64_t;
  auto appendValueToColumnWithKey(ColumnKey_t key, double value, joda::func::ParticleValidity) -> int64_t;
  auto appendValueToColumnWithKey(const std::string &rowName, ColumnKey_t key, double value,
                                  joda::func::ParticleValidity) -> int64_t;

  auto appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, double value, joda::func::ParticleValidity) -> int64_t;
  auto appendValueToColumnAtRowWithKey(ColumnKey_t key, int64_t rowIdx, double value, joda::func::ParticleValidity)
      -> int64_t;

  auto appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, joda::func::ParticleValidity,
                                joda::func::ParticleValidity validityValue) -> int64_t;
  auto appendValueToColumnAtRowWithKey(ColumnKey_t key, int64_t rowIdx, joda::func::ParticleValidity,
                                       joda::func::ParticleValidity validityValue) -> int64_t;

  auto getTable() const -> const Table_t &;
  auto getStatistics() const -> const std::map<uint64_t, Statistics> &;
  bool containsStatistics(uint64_t colIdx) const;
  auto getStatistics(uint64_t colIdx) const -> const Statistics &;

  auto getNrOfColumns() const -> int64_t;
  bool containsColumn(int64_t colIdx) const;
  auto getNrOfRows() const -> int64_t;
  auto getNrOfRowsAtColumn(int64_t colIdx) const -> int64_t;
  static std::string validityToString(joda::func::ParticleValidity val);
  bool columnKeyExists(ColumnKey_t key) const;
  uint64_t getColIndexFromKey(ColumnKey_t key) const;

private:
  /////////////////////////////////////////////////////
  Table_t mTable;
  std::map<uint64_t, Statistics> mStatistics;
  std::map<uint64_t, std::string> mRowNames;
  std::map<uint64_t, std::string> mColumnName;
  std::map<ColumnKey_t, uint64_t>
      mColumnKeys;    ///< Used to identify a column unique. The key is the keys, the value is the column index
  std::map<uint64_t, ColumnKey_t> mColumnKeysForIndex;
  int64_t mRows = 0;
  Statistics mEmptyStatistics;
  mutable std::mutex mWriteMutex;
  TableMeta mTableMeta;
};

}    // namespace joda::results
