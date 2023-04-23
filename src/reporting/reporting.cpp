

#include "reporting.h"
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

namespace joda::reporting {

int64_t Table::appendValueToColumn(uint64_t colIdx, float value)
{
  if(!mTable.contains(colIdx)) {
    mTable.emplace(colIdx, Row_t{});
  }

  auto newIndex            = mTable[colIdx].size();
  mTable[colIdx][newIndex] = Row{.value = value};
  mStatisitcs[colIdx].addValue(value);
  mRows = std::max(mRows, static_cast<int64_t>(newIndex) + 1);
  return newIndex;
}

int64_t Table::appendValueToColumn(const std::string &rowName, uint64_t colIdx, float value)
{
  auto newIndex = appendValueToColumn(colIdx, value);
  setRowName(newIndex, rowName);
  return newIndex;
}

auto Table::getNrOfColumns() const -> int64_t
{
  return mTable.size();
}

auto Table::getNrOfRows() const -> int64_t
{
  return mRows;
}

auto Table::getTable() const -> const Table_t &
{
  return mTable;
}
auto Table::getStatisitcs() const -> const std::map<uint64_t, Statistics> &
{
  return mStatisitcs;
}

void Table::setRowName(uint64_t rowIdx, const std::string &name)
{
  mRowNames.emplace(rowIdx, name);
}

void Table::setColumnNames(const std::map<uint64_t, std::string_view> &&colNames)
{
  mColumnName = std::move(colNames);
}

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
void Table::flushReportToFile(std::string_view fileName) const
{
  std::ofstream outFile;
  outFile.open(fileName.data());

  //
  // Write column header
  //
  std::string rowBuffer;
  int64_t columns = std::max(getNrOfColumns(), static_cast<int64_t>(mColumnName.size()));
  for(int64_t colIdx = 0; colIdx < columns; colIdx++) {
    if(0 == colIdx) {
      rowBuffer += CSV_SEPARATOR;
    }

    if(mColumnName.contains(colIdx)) {
      rowBuffer += std::string(mColumnName.at(colIdx)) + CSV_SEPARATOR;
    } else {
      rowBuffer += std::to_string(colIdx) + CSV_SEPARATOR;
    }
  }
  if(!rowBuffer.empty()) {
    rowBuffer.pop_back();    // Remove trailing CSV_SEPARATOR
  }
  rowBuffer += "\n";
  outFile << rowBuffer;

  //
  // Write table data
  //
  for(int64_t rowIdx = 0; rowIdx < getNrOfRows(); rowIdx++) {
    std::string rowBuffer = "";
    for(int64_t colIdx = 0; colIdx < getNrOfColumns(); colIdx++) {
      //
      // Write row data
      //
      if(0 == colIdx) {
        //
        // Write row header
        //
        if(mRowNames.contains(rowIdx)) {
          rowBuffer += std::string(mRowNames.at(rowIdx)) + CSV_SEPARATOR;
        } else {
          rowBuffer += std::to_string(rowIdx) + CSV_SEPARATOR;
        }
      }

      if(mTable.at(colIdx).contains(rowIdx)) {
        rowBuffer += std::to_string(mTable.at(colIdx).at(rowIdx).value) + CSV_SEPARATOR;
      } else {
        // Empty table entry
        rowBuffer += CSV_SEPARATOR;
      }
    }

    if(!rowBuffer.empty()) {
      rowBuffer.pop_back();    // Remove trailing CSV_SEPARATOR
    }
    rowBuffer += "\n";
    outFile << rowBuffer;
  }

  //
  // Write separator
  //
  rowBuffer = "";
  for(int64_t colIdx = 0; colIdx < columns; colIdx++) {
    if(0 == colIdx) {
      rowBuffer += CSV_SEPARATOR;
    }
    // Delimiter before summary stars
    rowBuffer += std::string("   ") + CSV_SEPARATOR;
  }
  if(!rowBuffer.empty()) {
    rowBuffer.pop_back();    // Remove trailing CSV_SEPARATOR
  }
  rowBuffer += "\n";
  outFile << rowBuffer;

  //
  // Write table statistircs
  //
  for(int n = 0; n < Statistics::NR_OF_VALUE; n++) {
    std::string rowBuffer = Statistics::getStatisitcsTitle()[n] + CSV_SEPARATOR;
    for(const auto &[_, statistics] : mStatisitcs) {
      rowBuffer += std::to_string(statistics.getStatisitcs()[n]) + CSV_SEPARATOR;
    }
    rowBuffer.pop_back();    // Remove trailing CSV_SEPARATOR
    rowBuffer += "\n";
    outFile << rowBuffer;
  }

  outFile.close();
}

auto Statistics::getStatisitcsTitle() -> const std::array<std::string, NR_OF_VALUE>
{
  return {"Nr", "Sum", "Min", "Max", "Mean"};
}
auto Statistics::getStatisitcs() const -> const std::array<float, NR_OF_VALUE>
{
  return {(float) mNr, mSum, mMin, mMax, mMean};
}
}    // namespace joda::reporting
