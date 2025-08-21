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
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>

namespace joda::table {

Table::Table() = default;

Table::Table(const std::vector<TableColumn> &input)
{
  for(size_t n = 0; n < input.size(); n++) {
    mDataColOrganized.emplace(n, input.at(n));
  }
}

void Table::setColHeader(const std::map<uint32_t, settings::ResultsSettings::ColumnKey> &data)
{
  for(const auto &[colIDx, key] : data) {
    mDataColOrganized[colIDx].colSettings = key;
  }
}

void Table::setColHeader(uint32_t colIdx, const settings::ResultsSettings::ColumnKey &data)
{
  mDataColOrganized[colIdx].colSettings = data;
}

void Table::setTitle(const std::string &title)
{
  mTitle = title;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Table::init(uint32_t cols, uint32_t rows)
{
  if(cols > 0 && rows > 0) {
    setData(rows - 1, cols - 1, {std::numeric_limits<double>::quiet_NaN(), {.isValid = false}, {}});
  }
}

void Table::clear()
{
  mDataColOrganized.clear();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] std::shared_ptr<const TableCell> Table::data(uint32_t row, uint32_t col) const
{
  if(mDataColOrganized.contains(col)) {
    if(mDataColOrganized.at(col).rows.contains(row)) {
      return mDataColOrganized.at(col).rows.at(row);
    }
  }
  return nullptr;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] std::shared_ptr<TableCell> Table::mutableData(uint32_t row, uint32_t col)
{
  if(mDataColOrganized.contains(col)) {
    if(mDataColOrganized.at(col).rows.contains(row)) {
      return mDataColOrganized.at(col).rows.at(row);
    }
  }
  return nullptr;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::pair<double, double> Table::getMinMax(uint32_t column) const
{
  double min      = std::numeric_limits<double>::max();
  double max      = std::numeric_limits<double>::min();
  const auto &col = mDataColOrganized.at(column);
  for(const auto &[_, row] : col.rows) {
    if(!row->isNAN() && row->isValid()) {
      if(row->getVal() < min) {
        min = row->getVal();
      }
      if(row->getVal() > max) {
        max = row->getVal();
      }
    }
  }
  return {min, max};
}

std::pair<double, double> Table::getMinMax() const
{
  double min = std::numeric_limits<double>::max();
  double max = std::numeric_limits<double>::min();
  for(const auto &[_1, col] : mDataColOrganized) {
    for(const auto &[_2, rowIn] : col.rows) {
      if(!rowIn->isNAN() && rowIn->isValid()) {
        if(rowIn->getVal() < min) {
          min = rowIn->getVal();
        }
        if(rowIn->getVal() > max) {
          max = rowIn->getVal();
        }
      }
    }
  }
  return {min, max};
}

}    // namespace joda::table
