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
#include "backend/image_processing/roi/roi.hpp"
#include <nlohmann/detail/macro_scope.hpp>
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
      nrTotal(other.nrTotal), nrInvalid(other.nrInvalid), sum(other.sum), min(other.min), max(other.max),
      mean(other.mean)
  {
  }

  Statistics &operator=(const Statistics other)
  {
    nrTotal   = other.nrTotal;
    nrInvalid = other.nrInvalid;
    sum       = other.sum;
    min       = other.min;
    max       = other.max;
    mean      = other.mean;

    return *this;
  }

  /////////////////////////////////////////////////////
  void addValue(double val)
  {
    std::lock_guard<std::mutex> lock(mAddMutex);
    if(nrTotal == 0) {
      min = val;
      max = val;
    } else {
      min = std::min(min, val);
      max = std::max(max, val);
    }
    nrTotal++;

    sum += val;
    mean = sum / static_cast<double>(nrTotal);
  }

  void incrementInvalid()
  {
    std::lock_guard<std::mutex> lock(mAddMutex);
    nrInvalid++;
  }

  void reset()
  {
    nrTotal = 0;
    sum     = 0;
    min     = 0;
    max     = 0;
    mean    = 0;
  }

  [[nodiscard]] uint64_t getNr() const
  {
    return nrTotal;
  }

  [[nodiscard]] uint64_t getInvalid() const
  {
    return nrInvalid;
  }

  [[nodiscard]] double getSum() const
  {
    return sum;
  }

  [[nodiscard]] double getMin() const
  {
    return min;
  }

  [[nodiscard]] double getMax() const
  {
    return max;
  }

  [[nodiscard]] double getAvg() const
  {
    return mean;
  }

  static auto getStatisticsTitle() -> const std::array<std::string, NR_OF_VALUE>;
  [[nodiscard]] auto getStatistics() const -> const std::array<double, NR_OF_VALUE>;

private:
  /////////////////////////////////////////////////////
  uint64_t nrTotal   = 0;
  uint64_t nrInvalid = 0;
  double sum         = 0;
  double min         = 0;
  double max         = 0;
  double mean        = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Statistics, nrTotal, nrInvalid, sum, min, max, mean);

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
  Table() = default;
  Table(const joda::results::Table &dataIn) :
      data(dataIn.data), meta(dataIn.meta), stats(dataIn.stats), rowNames(dataIn.rowNames), colNames(dataIn.colNames),
      colKeys(dataIn.colKeys), mRows(dataIn.mRows), mColumnKeys(dataIn.mColumnKeys)
  {
  }

  enum class ValidityState
  {
    INVALID = 0,
    VALID   = 1
  };

  struct Row
  {
    std::variant<double, joda::func::ParticleValidity> val;
    bool isValid;

    friend void to_json(nlohmann ::json &nlohmann_json_j, const Row &nlohmann_json_t)
    {
      if(std::holds_alternative<double>(nlohmann_json_t.val)) {
        nlohmann_json_j["val"] = std::get<double>(nlohmann_json_t.val);
      } else if(std::holds_alternative<joda::func::ParticleValidity>(nlohmann_json_t.val)) {
        nlohmann_json_j["val"] = std::get<joda::func::ParticleValidity>(nlohmann_json_t.val);
      }
      nlohmann_json_j["isValid"] = nlohmann_json_t.isValid;
    }
    friend void from_json(const nlohmann ::json &nlohmann_json_j, Row &nlohmann_json_t)
    {
      if(nlohmann_json_j.at("val").is_string()) {
        joda::func::ParticleValidity parseVal = nlohmann_json_j.at("val");
        nlohmann_json_t.val                   = parseVal;
      } else {
        double parseVal     = nlohmann_json_j.at("val");
        nlohmann_json_t.val = parseVal;
      }
      nlohmann_json_j.at("isValid").get_to(nlohmann_json_t.isValid);
    };
  };

  struct TableMeta
  {
    std::string tableName;
    joda::func::ResponseDataValidity validity   = joda::func::ResponseDataValidity::VALID;
    bool invalidateAllDataOnOneChannelIsInvalid = false;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TableMeta, tableName, validity, invalidateAllDataOnOneChannelIsInvalid);
  };

  /////////////////////////////////////////////////////
  using Row_t       = std::map<uint32_t, Row>;      // Row index
  using Table_t     = std::map<uint64_t, Row_t>;    // Column index
  using ColumnKey_t = int64_t;

  /////////////////////////////////////////////////////
  void setTableName(const std::string &name);
  void setTableValidity(joda::func::ResponseDataValidity valid, bool invalidWholeData);
  auto getTableValidity() const -> std::tuple<joda::func::ResponseDataValidity, bool>;

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

  auto appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, ValidityState isValid,
                                joda::func::ParticleValidity validityValue) -> int64_t;
  auto appendValueToColumnAtRowWithKey(ColumnKey_t key, int64_t rowIdx, ValidityState isValid,
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
  Table_t data;
  TableMeta meta;
  std::map<uint64_t, Statistics> stats;
  std::map<uint64_t, std::string> rowNames;
  std::map<uint64_t, std::string> colNames;
  ///< Used to identify a column unique. The key is the col index, the value is unique col key
  std::map<uint64_t, ColumnKey_t> colKeys;

  std::string configSchema = "https://imagec.org/schemas/v1/results.json";
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Table, configSchema, data, meta, stats, rowNames, colNames, colKeys);

  /// Helpers//////////////////////////////////////////
  int64_t mRows = 0;
  Statistics mEmptyStatistics;
  ///< Used to identify a column unique. The key is the key, the value is the column index
  std::map<ColumnKey_t, uint64_t> mColumnKeys;
  mutable std::mutex mWriteMutex;
};

}    // namespace joda::results
