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
void Table::setTitle(const std::string &title)
{
  mTitle = title;
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
std::pair<double, double> Table::getMinMax(int column) const
{
  double min = std::numeric_limits<double>::max();
  double max = std::numeric_limits<double>::min();
  auto &col  = mDataColOrganized.at(column);
  for(const auto &[_, row] : col.rows) {
    if(!row.isNAN() && row.isValid()) {
      if(row.getVal() < min) {
        min = row.getVal();
      }
      if(row.getVal() > max) {
        max = row.getVal();
      }
    }
  }
  return {min, max};
}

}    // namespace joda::table
