///
/// \file      reporting.h
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
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace joda::reporting {

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
  static constexpr int NR_OF_VALUE = 5;

  /////////////////////////////////////////////////////
  void addValue(float val)
  {
    if(mNr == 0) {
      mMin = val;
      mMax = val;
    } else {
      mMin = std::min(mMin, val);
      mMax = std::max(mMax, val);
    }
    mNr++;

    mSum += val;
    mMean = mSum / mNr;
  }

  void reset()
  {
    mNr   = 0;
    mSum  = 0;
    mMin  = 0;
    mMax  = 0;
    mMean = 0;
  }

  uint64_t getNr() const
  {
    return mNr;
  }

  float getSum() const
  {
    return mSum;
  }

  float getMin() const
  {
    return mMin;
  }

  float getMax() const
  {
    return mMax;
  }

  float getAvg() const
  {
    return mMean;
  }

  static auto getStatisticsTitle() -> const std::array<std::string, NR_OF_VALUE>;
  [[nodiscard]] auto getStatistics() const -> const std::array<float, NR_OF_VALUE>;

private:
  /////////////////////////////////////////////////////
  uint64_t mNr = 0;
  float mSum   = 0;
  float mMin   = 0;
  float mMax   = 0;
  float mMean  = 0;
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
    float value;
  };

  /////////////////////////////////////////////////////
  using Row_t   = std::map<uint32_t, Row>;
  using Table_t = std::map<uint64_t, Row_t>;

  /////////////////////////////////////////////////////
  void setColumnNames(const std::map<uint64_t, std::string> &);
  auto getColumnNameAt(uint64_t colIdx) const -> const std::string;
  void setRowName(uint64_t rowIdx, const std::string &);
  auto appendValueToColumn(uint64_t colIdx, float value) -> int64_t;
  auto appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, float value) -> int64_t;
  auto appendValueToColumn(const std::string &rowName, uint64_t colIdx, float value) -> int64_t;
  auto getTable() const -> const Table_t &;
  auto getStatistics() const -> const std::map<uint64_t, Statistics> &;
  auto getStatistics(uint64_t colIdx) const -> const Statistics &;
  void flushReportToFile(std::string_view fileName) const;
  auto getNrOfColumns() const -> int64_t;
  auto getNrOfRows() const -> int64_t;

private:
  /////////////////////////////////////////////////////
  Table_t mTable;
  std::map<uint64_t, Statistics> mStatisitcs;
  std::map<uint64_t, std::string> mRowNames;
  std::map<uint64_t, std::string> mColumnName;
  int64_t mRows = 0;
  Statistics mEmptyStatistics;
};

}    // namespace joda::reporting
