///
/// \file      table.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-20
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
#include <map>
#include <string>
#include <vector>

namespace joda::results {

class TableCell
{
public:
  /////////////////////////////////////////////////////
  TableCell()
  {
  }

  TableCell(double val, bool valid) : value(val), validity(valid)
  {
  }

  [[nodiscard]] double getVal() const
  {
    return value;
  }

  [[nodiscard]] bool isValid() const
  {
    return validity;
  }

private:
  /////////////////////////////////////////////////////
  double value  = 0;
  bool validity = true;
};

using entry_t = std::map<uint32_t, std::map<uint32_t, TableCell>>;

///
/// \class      Table
/// \author     Joachim Danmayr
/// \brief
///
class Table
{
public:
  /////////////////////////////////////////////////////
  Table();
  void setColHeader(const std::map<uint32_t, std::string> &);
  void setRowHeader(const std::map<uint32_t, std::string> &);
  auto getMutableRowHeader() -> std::map<uint32_t, std::string> &
  {
    return mRowHeader;
  }
  auto getMutableColHeader() -> std::map<uint32_t, std::string> &
  {
    return mColHeader;
  }
  void print();
  [[nodiscard]] const entry_t &data() const
  {
    return mData;
  }

  void setData(uint32_t row, uint32_t col, const TableCell &data)
  {
    if(data.isValid()) {
      mMin = std::min(mMin, data.getVal());
      mMax = std::max(mMax, data.getVal());
    }

    mData[row][col] = data;
  }

  [[nodiscard]] auto getMinMax() const -> std::tuple<double, double>
  {
    return {mMin, mMax};
  }

  uint32_t getRows()
  {
    return mData.size();
  }
  uint32_t getCols()
  {
    return mColHeader.size();
  }

private:
  /////////////////////////////////////////////////////
  entry_t mData;    // <ROW, <COL, DATA>>
  double mMin = std::numeric_limits<double>::max();
  double mMax = std::numeric_limits<double>::min();

  std::map<uint32_t, std::string> mColHeader;
  std::map<uint32_t, std::string> mRowHeader;
};

}    // namespace joda::results
