

#include "reporting.h"
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

namespace joda::reporting {

void Table::setTableName(const std::string &name)
{
  mTableName = name;
}

const std::string &Table::getTableName() const
{
  return mTableName;
}

int64_t Table::appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, float value,
                                        joda::func::ParticleValidity validity)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);

  if(!mTable.contains(colIdx)) {
    mTable.emplace(colIdx, Row_t{});
  }

  if(rowIdx < 0) {
    rowIdx = mTable[colIdx].size();
  }

  mTable[colIdx][rowIdx] = Row{.value = value};

  // Only count valid particles
  if(joda::func::ParticleValidity::VALID == validity) {
    mStatistics[colIdx].addValue(value);
  } else {
    mStatistics[colIdx].incrementInvalid();
  }

  mRows = std::max(mRows, static_cast<int64_t>(rowIdx) + 1);
  return rowIdx;
}

auto Table::appendValueToColumnAtRow(uint64_t colIdx, int64_t rowIdx, joda::func::ParticleValidity value) -> int64_t
{
  std::lock_guard<std::mutex> lock(mWriteMutex);

  if(!mTable.contains(colIdx)) {
    mTable.emplace(colIdx, Row_t{});
  }

  if(rowIdx < 0) {
    rowIdx = mTable[colIdx].size();
  }

  mTable[colIdx][rowIdx] = Row{.validity = value};

  // Only count valid particles
  if(joda::func::ParticleValidity::VALID == value) {
    mStatistics[colIdx].incrementInvalid();
  }

  mRows = std::max(mRows, static_cast<int64_t>(rowIdx) + 1);
  return rowIdx;
}

int64_t Table::appendValueToColumn(uint64_t colIdx, float value, joda::func::ParticleValidity validity)
{
  return appendValueToColumnAtRow(colIdx, -1, value, validity);
}

int64_t Table::appendValueToColumn(const std::string &rowName, uint64_t colIdx, float value,
                                   joda::func::ParticleValidity validity)
{
  auto newIndex = appendValueToColumn(colIdx, value, validity);
  setRowName(newIndex, rowName);
  return newIndex;
}

auto Table::getNrOfColumns() const -> int64_t
{
  return std::max(static_cast<int64_t>(mTable.size()), static_cast<int64_t>(mColumnName.size()));
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

void Table::setRowName(uint64_t rowIdx, const std::string &name)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  mRowNames.emplace(rowIdx, name);
}

void Table::setColumnNames(const std::map<uint64_t, std::string> &colNames)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  for(const auto &[key, val] : colNames) {
    mColumnName.emplace(key, val);
  }
}

auto Table::getColumnNameAt(uint64_t colIdx) const -> const std::string
{
  return mColumnName.at(colIdx);
}

auto Statistics::getStatisticsTitle() -> const std::array<std::string, NR_OF_VALUE>
{
  return {"Valid", "Invalid", "Sum", "Min", "Max", "Avg"};
}
auto Statistics::getStatistics() const -> const std::array<float, NR_OF_VALUE>
{
  return {(float) mNr, (float) mInvalid, mSum, mMin, mMax, mMean};
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

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::REFERENCE_SPOT) ==
     joda::func::ParticleValidity::REFERENCE_SPOT) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "ref spot.";
  }
  return ret;
}

}    // namespace joda::reporting
