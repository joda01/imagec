///
/// \file      table.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "table.hpp"
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>

namespace joda::table {

Table::Table()
{
}

void Table::print()
{
  using namespace std;
  // Find maximum column width for each inner map
  std::vector<unsigned long> colWidths(mData.begin()->second.size());    // Initialize with inner map size
  for(const auto &outerPair : mData) {
    int innerIdx = 0;
    for(const auto &innerPair : outerPair.second) {
      colWidths[innerIdx] = max(colWidths[innerIdx], (unsigned long) to_string(innerPair.second.getVal()).size());
      innerIdx++;
    }
  }

  // Print header row with proper formatting
  std::cout << "O";
  int n = 0;
  for(unsigned long colWidth : colWidths) {
    cout << right << setw(colWidth + 2) << mColHeader[n];
    n++;
  }
  cout << endl;

  // Print separator row
  cout << string(10, '-') << right;    // Adjust width as needed
  for(unsigned long width : colWidths) {
    cout << setw(width + 4) << '-' << right;
  }
  cout << endl;

  // Print data rows with alignment and precision
  for(const auto &outerPair : mData) {
    cout << mRowHeader[outerPair.first];
    for(const auto &innerPair : outerPair.second) {
      if(colWidths.size() > innerPair.first) {
        cout << right << setw(colWidths[innerPair.first] + 2) << fixed << setprecision(4) << innerPair.second.getVal();
      }
    }
    cout << endl;
  }
}

void Table::setColHeader(const std::map<uint32_t, std::string> &data)
{
  mColHeader = data;
}
void Table::setRowHeader(const std::map<uint32_t, std::string> &data)
{
  mRowHeader = data;
}

void Table::setRowName(uint32_t row, const std::string &data)
{
  mRowHeader[row] = data;
}

void Table::setTitle(const std::string &title)
{
  mTitle = title;
}

void Table::setMeta(const Meta &meta)
{
  mMeta = meta;
}

void Table::clear()
{
  mData.clear();
  mColHeader.clear();
  mRowHeader.clear();
  mNrOfCols = 0;
  mTitle.clear();
  mMeta = {};
  mMin  = std::numeric_limits<double>::max();
  mMax  = std::numeric_limits<double>::min();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] TableCell Table::data(uint32_t row, uint32_t col) const
{
  if(mData.contains(row)) {
    if(mData.at(row).contains(col)) {
      return mData.at(row).at(col);
    }
  }
  return {};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Table::arrangeByTrackingId()
{
  std::map<uint64_t, int32_t> trackingIdRowIdx;    // <TRACKING ID, ROW IDX>
  std::map<uint64_t, int32_t> objectIdRowIdx;      // <OBJECT ID ID, ROW IDX>

  int32_t trackingIdRow = 0;
  std::map<int32_t, int32_t> objectIdRow;    // <COL ID, COL ROW IDX CNT>

  for(const auto &[rowIdx, row] : mData) {
    for(const auto &[colIdx, cell] : row) {
      if(cell.getTrackingId() != 0) {
        if(!trackingIdRowIdx.contains(cell.getTrackingId())) {
          trackingIdRowIdx.emplace(cell.getTrackingId(), trackingIdRow);
          trackingIdRow++;
        }
      } else {
        if(!objectIdRowIdx.contains(cell.getId())) {
          objectIdRowIdx.emplace(cell.getId(), objectIdRow[colIdx]);
          objectIdRow[colIdx]++;
        }
      }
    }
  }

  entry_t orderedTable;
  for(const auto &[_, row] : mData) {
    for(const auto &[colIdx, cell] : row) {
      int32_t rowIdx = -1;
      if(cell.getTrackingId() != 0) {
        rowIdx = trackingIdRowIdx.at(cell.getTrackingId());
      } else {
        rowIdx = objectIdRowIdx.at(cell.getId());
      }
      orderedTable[rowIdx][colIdx] = cell;
    }
  }
  mData = orderedTable;
}

}    // namespace joda::table
