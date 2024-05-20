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

#include <map>
#include <string>
#include <vector>

namespace joda::results {

using entry_t = std::map<uint32_t, std::map<uint32_t, double>>;

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
  entry_t &data()
  {
    return mData;
  }

private:
  /////////////////////////////////////////////////////
  entry_t mData;    // <ROW, <COL, DATA>>

  std::map<uint32_t, std::string> mColHeader;
  std::map<uint32_t, std::string> mRowHeader;
};

}    // namespace joda::results
