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

///

#pragma once

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "backend/helper/helper.hpp"

namespace joda::table {

class TableCell
{
public:
  /////////////////////////////////////////////////////
  TableCell()
  {
  }

  TableCell(double val, uint64_t id, bool valid, const std::string &linkToImage) : value(val), id(id), validity(valid), linkToImage(linkToImage)
  {
  }

  TableCell(double val, uint64_t id, const std::string &linkToImage) : value(val), id(id), linkToImage(linkToImage)
  {
  }

  [[nodiscard]] double getVal() const
  {
    return value;
  }

  [[nodiscard]] uint64_t getId() const
  {
    return id;
  }

  [[nodiscard]] bool isValid() const
  {
    return validity;
  }

  [[nodiscard]] const std::filesystem::path &getControlImagePath() const
  {
    return linkToImage;
  }

  [[nodiscard]] bool isNAN() const
  {
    return std::isnan(value);
  }

private:
  /////////////////////////////////////////////////////
  double value  = std::numeric_limits<double>::quiet_NaN();
  uint64_t id   = 0;
  bool validity = true;
  std::filesystem::path linkToImage;
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
  struct Meta
  {
    std::string classsName;
    std::string className;
  };

  Table();
  void setTitle(const std::string &title);
  void setMeta(const Meta &);
  void setColHeader(const std::map<uint32_t, std::string> &);
  void setRowHeader(const std::map<uint32_t, std::string> &);
  void setRowName(uint32_t row, const std::string &data);
  auto getMutableRowHeader() -> std::map<uint32_t, std::string> &
  {
    return mRowHeader;
  }
  auto getMutableColHeader() -> std::map<uint32_t, std::string> &
  {
    return mColHeader;
  }

  [[nodiscard]] auto getRowHeader(uint32_t idx) const -> std::string
  {
    if(mRowHeader.contains(idx)) {
      return mRowHeader.at(idx);
    }
    return "";
  }
  [[nodiscard]] auto getColHeader(uint32_t idx) const -> std::string
  {
    if(mColHeader.contains(idx)) {
      return mColHeader.at(idx);
    }
    return "";
  }

  void print();
  [[nodiscard]] TableCell data(uint32_t row, uint32_t col) const;

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

  [[nodiscard]] auto getAvg(double *sizeOut = nullptr) const -> double
  {
    double sum  = 0.0;
    double size = 0;
    for(int row = 0; row < getRows(); row++) {
      for(int col = 0; col < getCols(); col++) {
        if(data(row, col).isValid() && !data(row, col).isNAN()) {
          sum += data(row, col).getVal();
          size++;
        }
      }
    }
    if(sizeOut != nullptr) {
      *sizeOut = size;
    }
    return sum / size;
  }

  [[nodiscard]] auto getStddev() const -> double
  {
    double size    = 0;
    double average = getAvg(&size);
    if(size <= 1) {
      return 0.0;    // Handle case of empty or single-element array
    }
    double variance = 0.0;
    for(int row = 0; row < getRows(); row++) {
      for(int col = 0; col < getCols(); col++) {
        if(data(row, col).isValid() && !data(row, col).isNAN()) {
          variance += pow(data(row, col).getVal() - average, 2);
        }
      }
    }
    return sqrt(variance / (size - 1));    // Use unbiased sample standard deviation
  }

  [[nodiscard]] uint32_t getRows() const
  {
    return mData.size();
  }
  [[nodiscard]] uint32_t getCols() const
  {
    return std::max(mNrOfCols, getColHeaderSize());
  }

  [[nodiscard]] uint32_t getRowHeaderSize() const
  {
    return mRowHeader.size();
  }
  [[nodiscard]] uint32_t getColHeaderSize() const
  {
    return mColHeader.size();
  }

  [[nodiscard]] std::string getTitle() const
  {
    return mTitle;
  }

  [[nodiscard]] const Meta &getMeta() const
  {
    return mMeta;
  }

private:
  /////////////////////////////////////////////////////
  entry_t mData;    // <ROW, <COL, DATA>>
  double mMin = std::numeric_limits<double>::max();
  double mMax = std::numeric_limits<double>::min();

  std::map<uint32_t, std::string> mColHeader;
  std::map<uint32_t, std::string> mRowHeader;
  uint32_t mNrOfCols = 0;
  std::string mTitle;
  Meta mMeta;
};

}    // namespace joda::table
