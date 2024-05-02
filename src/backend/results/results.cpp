

#include "results.h"
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

namespace joda::results {

void Table::setTableName(const std::string &name)
{
  meta.tableName = name;
}

const std::string &Table::getTableName() const
{
  return meta.tableName;
}

void Table::setTableValidity(joda::func::ResponseDataValidity valid, bool invalidWholeData)
{
  meta.validity                               = valid;
  meta.invalidateAllDataOnOneChannelIsInvalid = invalidWholeData;
}

auto Table::getTableValidity() const -> std::tuple<joda::func::ResponseDataValidity, bool>
{
  return {meta.validity, meta.invalidateAllDataOnOneChannelIsInvalid};
}

auto Table::getNrOfRowsAtColumn(int64_t colIdx) const -> int64_t
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  if(data.contains(colIdx)) {
    return data.at(colIdx).size();
  }
  return 0;
}

bool Table::columnKeyExists(ColumnKey_t key) const
{
  return mColumnKeys.contains(key);
}

uint64_t Table::getColIndexFromKey(ColumnKey_t key) const
{
  return mColumnKeys.at(key);
}

int64_t Table::appendValueToColumnAtRowWithKey(ColumnKey_t key, int64_t rowIdx, double value,
                                               joda::func::ParticleValidity validity)
{
  return appendValueToColumnAtRow(getColIndexFromKey(key), rowIdx, value, validity);
}

int64_t Table::appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, double value,
                                        joda::func::ParticleValidity validity)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);

  if(!data.contains(colIdx)) {
    data.emplace(colIdx, Row_t{});
  }

  if(rowIdx < 0) {
    rowIdx = data[colIdx].size();
  }

  data[colIdx][rowIdx] = Row{.val = value, .isValid = joda::func::ParticleValidity::VALID == validity};

  // Only count valid particles
  if(joda::func::ParticleValidity::VALID == validity) {
    stats[colIdx].addValue(value);
  } else {
    stats[colIdx].incrementInvalid();
  }

  mRows = std::max(mRows, static_cast<int64_t>(rowIdx) + 1);
  return rowIdx;
}

auto Table::appendValueToColumnAtRowWithKey(ColumnKey_t key, int64_t rowIdx, ValidityState isValid,
                                            joda::func::ParticleValidity validityValue) -> int64_t
{
  return appendValueToColumnAtRow(getColIndexFromKey(key), rowIdx, isValid, validityValue);
}

auto Table::appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, ValidityState isValid,
                                     joda::func::ParticleValidity validityValue) -> int64_t
{
  std::lock_guard<std::mutex> lock(mWriteMutex);

  if(!data.contains(colIdx)) {
    data.emplace(colIdx, Row_t{});
  }

  if(rowIdx < 0) {
    rowIdx = data[colIdx].size();
  }

  data[colIdx][rowIdx] = Row{.val = validityValue, .isValid = isValid == ValidityState::VALID};

  // Only count valid particles

  if(isValid == ValidityState::INVALID) {
    stats[colIdx].incrementInvalid();
    stats[colIdx].addValue(0);
  } else {
    stats[colIdx].addValue(1);
  }

  mRows = std::max(mRows, static_cast<int64_t>(rowIdx) + 1);
  return rowIdx;
}

int64_t Table::appendValueToColumnWithKey(ColumnKey_t key, double value, joda::func::ParticleValidity validity)
{
  return appendValueToColumn(mColumnKeys.at(key), value, validity);
}

int64_t Table::appendValueToColumn(uint64_t colIdx, double value, joda::func::ParticleValidity validity)
{
  return appendValueToColumnAtRow(colIdx, -1, value, validity);
}

auto Table::appendValueToColumnWithKey(const std::string &rowName, ColumnKey_t key, double value,
                                       joda::func::ParticleValidity validity) -> int64_t
{
  return appendValueToColumnWithKey(rowName, mColumnKeys.at(key), value, validity);
}

int64_t Table::appendValueToColumn(const std::string &rowName, uint64_t colIdx, double value,
                                   joda::func::ParticleValidity validity)
{
  auto newIndex = appendValueToColumn(colIdx, value, validity);
  setRowName(newIndex, rowName);
  return newIndex;
}

bool Table::containsColumn(int64_t colIdx) const
{
  return data.contains(colIdx);
}

auto Table::getNrOfColumns() const -> int64_t
{
  return std::max(static_cast<int64_t>(data.size()), static_cast<int64_t>(colNames.size()));
}

auto Table::getRowNames() const -> const std::map<uint64_t, std::string> &
{
  return rowNames;
}

auto Table::getNrOfRows() const -> int64_t
{
  return mRows;
}

auto Table::getTable() const -> const Table_t &
{
  return data;
}
auto Table::getStatistics() const -> const std::map<uint64_t, Statistics> &
{
  return stats;
}

auto Table::getStatistics(uint64_t colIdx) const -> const Statistics &
{
  if(stats.contains(colIdx)) {
    return stats.at(colIdx);
  }
  return mEmptyStatistics;
}

bool Table::containsStatistics(uint64_t colIdx) const
{
  return stats.contains(colIdx);
}

void Table::setRowName(uint64_t rowIdx, const std::string &name)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  rowNames.emplace(rowIdx, name);
}

void Table::setColumnName(uint64_t idx, const std::string &colName, ColumnKey_t key)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  colNames[idx]    = colName;
  colKeys[idx]     = key;
  mColumnKeys[key] = idx;
}

auto Table::getColumnNameAt(uint64_t colIdx) const -> const std::string
{
  if(colNames.contains(colIdx)) {
    return colNames.at(colIdx);
  }
  return std::to_string(colIdx);
}

auto Table::getColumnKeyAt(uint64_t colIdx) const -> ColumnKey_t
{
  if(colKeys.contains(colIdx)) {
    return colKeys.at(colIdx);
  }
  return -1;
}

auto Table::getRowNameAt(uint64_t rowIdx) const -> const std::string
{
  if(rowNames.contains(rowIdx)) {
    return rowNames.at(rowIdx);
  }
  return std::to_string(rowIdx);
}

auto Statistics::getStatisticsTitle() -> const std::array<std::string, NR_OF_VALUE>
{
  return {"Valid", "Invalid", "Sum", "Min", "Max", "Avg"};
}
auto Statistics::getStatistics() const -> const std::array<double, NR_OF_VALUE>
{
  return {static_cast<double>(nrTotal), static_cast<double>(nrInvalid), sum, min, max, mean};
}

std::string Table::validityToString(joda::func::ParticleValidity val)
{
  if(val == joda::func::ParticleValidity::UNKNOWN) {
    return "-";
  }
  if(val == joda::func::ParticleValidity::VALID) {
    return "valid";
  }
  std::string ret;
  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::TOO_BIG) ==
     joda::func::ParticleValidity::TOO_BIG) {
    ret += "size(big)";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::TOO_SMALL) ==
     joda::func::ParticleValidity::TOO_SMALL) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "size(small)";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::TOO_LESS_OVERLAPPING) ==
     joda::func::ParticleValidity::TOO_LESS_OVERLAPPING) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "intersect too small";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::TOO_LESS_CIRCULARITY) ==
     joda::func::ParticleValidity::TOO_LESS_CIRCULARITY) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "circ.";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::AT_THE_EDGE) ==
     joda::func::ParticleValidity::AT_THE_EDGE) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "edge";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::REFERENCE_SPOT) ==
     joda::func::ParticleValidity::REFERENCE_SPOT) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "ref spot.";
  }
  return ret;
}

}    // namespace joda::results
