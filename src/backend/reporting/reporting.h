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
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include "../image_processing/detection/detection_response.hpp"
#include "xlsxwriter.h"

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
    double value;
    std::optional<joda::func::ParticleValidity> validity;
  };

  /////////////////////////////////////////////////////
  using Row_t   = std::map<uint32_t, Row>;      // Row index
  using Table_t = std::map<uint64_t, Row_t>;    // Column index

  /////////////////////////////////////////////////////
  void setTableName(const std::string &name);
  void setColumnNames(const std::map<uint64_t, std::string> &);
  void setColumnName(uint64_t idx, const std::string &colName);
  const std::string &getTableName() const;
  auto getColumnNameAt(uint64_t colIdx) const -> const std::string;
  void setRowName(uint64_t rowIdx, const std::string &);
  auto appendValueToColumn(uint64_t colIdx, double value, joda::func::ParticleValidity) -> int64_t;
  auto appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, double value, joda::func::ParticleValidity) -> int64_t;
  auto appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, joda::func::ParticleValidity) -> int64_t;

  auto appendValueToColumn(const std::string &rowName, uint64_t colIdx, double value, joda::func::ParticleValidity)
      -> int64_t;
  auto getTable() const -> const Table_t &;
  auto getStatistics() const -> const std::map<uint64_t, Statistics> &;
  bool containsStatistics(uint64_t colIdx) const;
  auto getStatistics(uint64_t colIdx) const -> const Statistics &;
  std::tuple<int, int> flushReportToFileXlsx(int colOffset, int /*rowOffset*/, lxw_worksheet *worksheet,
                                             lxw_format *header, lxw_format *merge_format,
                                             lxw_format *numberFormat) const;
  std::tuple<int, int> flushReportToFileXlsxTransponded(const std::string &headerText, int colOffset, int rowOffset,
                                                        int startRow, lxw_worksheet *worksheet, lxw_format *header,
                                                        lxw_format *merge_format, lxw_format *numberFormat,
                                                        lxw_format *imageHeaderHyperlinkFormat) const;
  auto getNrOfColumns() const -> int64_t;
  auto getNrOfRows() const -> int64_t;
  auto getNrOfRowsAtColumn(int64_t colIdx) const -> int64_t;

private:
  /////////////////////////////////////////////////////
  static std::string validityToString(joda::func::ParticleValidity val);

  /////////////////////////////////////////////////////
  Table_t mTable;
  std::map<uint64_t, Statistics> mStatistics;
  std::map<uint64_t, std::string> mRowNames;
  std::map<uint64_t, std::string> mColumnName;
  int64_t mRows = 0;
  Statistics mEmptyStatistics;
  mutable std::mutex mWriteMutex;
  std::string mTableName;
};

}    // namespace joda::reporting
