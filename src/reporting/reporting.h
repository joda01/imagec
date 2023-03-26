///
/// \file      reporting.h
/// \author    Joachim Danmayr
/// \date      2023-03-24
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
#include <cstdint>
#include <map>
#include <vector>

namespace joda::reporting {

class Statistics
{
public:
  /////////////////////////////////////////////////////
  void addValue(float val)
  {
    mSum += val;
    mQuadratSum = mQuadratSum + val * val;

    if(mNr == 0) {
      mMin = val;
      mMax = val;
    } else {
      mMin = std::min(mMin, val);
      mMax = std::max(mMax, val);
    }
    mNr++;
  }

  void reset()
  {
    mNr         = 0;
    mSum        = 0;
    mQuadratSum = 0;
    mMin        = 0;
    mMax        = 0;
  }

  uint64_t getNr() const
  {
    return mNr;
  }

  float getSum() const
  {
    return mSum;
  }

  float getQuadratSum() const
  {
    return mQuadratSum;
  }

  float getMin() const
  {
    return mMin;
  }

  float getMax() const
  {
    return mMax;
  }

private:
  /////////////////////////////////////////////////////
  uint64_t mNr      = 0;
  float mSum        = 0;
  float mQuadratSum = 0;
  float mMin        = 0;
  float mMax        = 0;
};

///
/// \class      Table
/// \author     Joachim Danmayr
/// \brief      A table representation
///
class Table
{
public:
  /////////////////////////////////////////////////////
  void appendValueToColumn(uint64_t colIdx, float value);
  auto getTable() const -> const std::map<uint64_t, std::map<uint32_t, float>> &;
  auto getStatisitcs() const -> const std::map<uint64_t, Statistics> &;

private:
  /////////////////////////////////////////////////////
  std::map<uint64_t, std::map<uint32_t, float>> mTable;
  std::map<uint64_t, Statistics> mStatisitcs;
};

}    // namespace joda::reporting
