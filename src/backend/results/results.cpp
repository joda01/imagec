

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
  mTableMeta.tableName = name;
}

const std::string &Table::getTableName() const
{
  return mTableMeta.tableName;
}

void Table::setTableValidity(joda::func::ResponseDataValidity valid, bool invalidWholeData)
{
  mTableMeta.validity                       = valid;
  mTableMeta.invalidWholeDataOnChannelError = invalidWholeData;
}

auto Table::getTableValidity() const -> std::tuple<joda::func::ResponseDataValidity, bool>
{
  return {mTableMeta.validity, mTableMeta.invalidWholeDataOnChannelError};
}

auto Table::getNrOfRowsAtColumn(int64_t colIdx) const -> int64_t
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  if(mTable.contains(colIdx)) {
    return mTable.at(colIdx).size();
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

  if(!mTable.contains(colIdx)) {
    mTable.emplace(colIdx, Row_t{});
  }

  if(rowIdx < 0) {
    rowIdx = mTable[colIdx].size();
  }

  mTable[colIdx][rowIdx] = Row{.value = value, .validity = validity};

  // Only count valid particles
  if(joda::func::ParticleValidity::VALID == validity) {
    mStatistics[colIdx].addValue(value);
  } else {
    mStatistics[colIdx].incrementInvalid();
  }

  mRows = std::max(mRows, static_cast<int64_t>(rowIdx) + 1);
  return rowIdx;
}

auto Table::appendValueToColumnAtRowWithKey(ColumnKey_t key, int64_t rowIdx, joda::func::ParticleValidity validity,
                                            joda::func::ParticleValidity validityValue) -> int64_t
{
  return appendValueToColumnAtRow(getColIndexFromKey(key), rowIdx, validity, validityValue);
}

auto Table::appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, joda::func::ParticleValidity validity,
                                     joda::func::ParticleValidity validityValue) -> int64_t
{
  std::lock_guard<std::mutex> lock(mWriteMutex);

  if(!mTable.contains(colIdx)) {
    mTable.emplace(colIdx, Row_t{});
  }

  if(rowIdx < 0) {
    rowIdx = mTable[colIdx].size();
  }

  mTable[colIdx][rowIdx] = Row{.value = validityValue, .validity = validity};

  // Only count valid particles

  if(joda::func::ParticleValidity::VALID != validity) {
    mStatistics[colIdx].incrementInvalid();
    mStatistics[colIdx].addValue(0);
  } else {
    mStatistics[colIdx].addValue(1);
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
  return mTable.contains(colIdx);
}

auto Table::getNrOfColumns() const -> int64_t
{
  return std::max(static_cast<int64_t>(mTable.size()), static_cast<int64_t>(mColumnName.size()));
}

auto Table::getRowNames() const -> const std::map<uint64_t, std::string> &
{
  return mRowNames;
}

auto Table::getNrOfRows() const -> int64_t
{
  return mRows;
}

auto Table::getTable() const -> const Table_t &
{
  return mTable;
}
auto Table::getStatistics() const -> const std::map<uint64_t, Statistics> &
{
  return mStatistics;
}

auto Table::getStatistics(uint64_t colIdx) const -> const Statistics &
{
  if(mStatistics.contains(colIdx)) {
    return mStatistics.at(colIdx);
  } else {
    return mEmptyStatistics;
  }
}

bool Table::containsStatistics(uint64_t colIdx) const
{
  return mStatistics.contains(colIdx);
}

void Table::setRowName(uint64_t rowIdx, const std::string &name)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  mRowNames.emplace(rowIdx, name);
}

void Table::setColumnName(uint64_t idx, const std::string &colName, ColumnKey_t key)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  mColumnName[idx]         = colName;
  mColumnKeysForIndex[idx] = key;
  mColumnKeys[key]         = idx;
}

auto Table::getColumnNameAt(uint64_t colIdx) const -> const std::string
{
  if(mColumnName.contains(colIdx)) {
    return mColumnName.at(colIdx);
  }
  return std::to_string(colIdx);
}

auto Table::getColumnKeyAt(uint64_t colIdx) const -> ColumnKey_t
{
  if(mColumnKeysForIndex.contains(colIdx)) {
    return mColumnKeysForIndex.at(colIdx);
  }
  return -1;
}

auto Table::getRowNameAt(uint64_t rowIdx) const -> const std::string
{
  if(mRowNames.contains(rowIdx)) {
    return mRowNames.at(rowIdx);
  }
  return std::to_string(rowIdx);
}

auto Statistics::getStatisticsTitle() -> const std::array<std::string, NR_OF_VALUE>
{
  return {"Valid", "Invalid", "Sum", "Min", "Max", "Avg"};
}
auto Statistics::getStatistics() const -> const std::array<double, NR_OF_VALUE>
{
  return {static_cast<double>(mNr), static_cast<double>(mInvalid), mSum, mMin, mMax, mMean};
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
