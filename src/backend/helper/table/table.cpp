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

void Table::setColHeader(const std::map<uint32_t, settings::ResultsSettings::ColumnKey> &data)
{
  for(const auto &[colIDx, key] : data) {
    mDataColOrganized[colIDx].colSettings = key;
  }
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
  mDataColOrganized.clear();
  mRowHeader.clear();
  mNrOfCols = 0;
  mNrOfRows = 0;
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
  if(mDataColOrganized.contains(col)) {
    if(mDataColOrganized.at(col).rows.contains(row)) {
      return mDataColOrganized.at(col).rows.at(row);
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

  for(const auto &[colIdx, col] : mDataColOrganized) {
    for(const auto &[rowIdx, cell] : col.rows) {
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
  for(const auto &[colIdx, col] : mDataColOrganized) {
    for(const auto &[_, cell] : col.rows) {
      int32_t rowIdx = -1;
      if(cell.getTrackingId() != 0) {
        rowIdx = trackingIdRowIdx.at(cell.getTrackingId());
      } else {
        rowIdx = objectIdRowIdx.at(cell.getId());
      }
      orderedTable[colIdx].rows[rowIdx] = cell;
    }
  }
  mDataColOrganized = orderedTable;
}

}    // namespace joda::table
