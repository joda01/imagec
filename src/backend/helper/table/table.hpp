///
/// \file      table.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "backend/enums/bigtypes.hpp"
#include "backend/helper/helper.hpp"
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::table {

using namespace joda::stdi;

class TableCell
{
public:
  struct MetaData
  {
    uint64_t objectIdGroup  = 0;    // Elements with the same object ID group should be printed side by side
    uint64_t objectId       = 0;    // The original object id
    uint64_t parentObjectId = 0;
    uint64_t trackingId     = 0;
    bool isValid            = true;
    uint32_t tStack         = 0;
    uint32_t zStack         = 0;
    uint32_t cStack         = 0;
  };

  struct Grouping
  {
    uint64_t groupIdx = 0;
    uint32_t posX     = 0;
    uint32_t posY     = 0;
  };

  /////////////////////////////////////////////////////
  TableCell()
  {
  }

  TableCell(double val, const MetaData &meta, const Grouping &grouping) : value(val), mMetaData(meta), mGrouping(grouping)
  {
  }

  [[nodiscard]] double getVal() const
  {
    return value;
  }

  [[nodiscard]] uint64_t getId() const
  {
    return mMetaData.objectIdGroup;
  }

  [[nodiscard]] uint64_t getObjectId() const
  {
    return mMetaData.objectId;
  }

  void setId(uint64_t id)
  {
    mMetaData.objectIdGroup = id;
  }

  [[nodiscard]] uint64_t getParentId() const
  {
    return mMetaData.parentObjectId;
  }

  [[nodiscard]] uint64_t getTrackingId() const
  {
    return mMetaData.trackingId;
  }

  [[nodiscard]] bool isValid() const
  {
    return mMetaData.isValid;
  }

  [[nodiscard]] bool isNAN() const
  {
    return std::isnan(value);
  }

  uint32_t getPosX() const
  {
    return mGrouping.posX;
  }

  uint32_t getPosY() const
  {
    return mGrouping.posY;
  }

  uint64_t getGroupId() const
  {
    return mGrouping.groupIdx;
  }

  uint32_t getStackT() const
  {
    return mMetaData.tStack;
  }

private:
  /////////////////////////////////////////////////////
  double value = std::numeric_limits<double>::quiet_NaN();
  MetaData mMetaData;
  Grouping mGrouping;
};

using colRows_t = std::map<uint32_t, TableCell>;    // This is a column with its rows

struct TableColumn
{
  colRows_t rows;
  settings::ResultsSettings::ColumnKey colSettings;
  std::string title;
};

using entry_t = std::map<uint32_t, TableColumn>;    // These are the different columns

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
    std::string className;
  };

  enum class SortFields
  {
    OBJECT_ID,
    TRACKING_ID
  };

  Table();
  void setTitle(const std::string &title);
  void setMeta(const Meta &);
  void setColHeader(const std::map<uint32_t, settings::ResultsSettings::ColumnKey> &);
  void setRowHeader(const std::map<uint32_t, std::string> &);
  void setRowName(uint32_t row, const std::string &data);
  auto getMutableRowHeader() -> std::map<uint32_t, std::string> &
  {
    return mRowHeader;
  }
  auto getMutableColHeader() -> entry_t &
  {
    return mDataColOrganized;
  }

  auto columns() const -> const entry_t &
  {
    return mDataColOrganized;
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
    if(mDataColOrganized.contains(idx)) {
      return mDataColOrganized.at(idx).colSettings.createHeader();
    }
    return "";
  }

  [[nodiscard]] TableCell data(uint32_t row, uint32_t col) const;

  void setData(uint32_t row, uint32_t col, const TableCell &data)
  {
    if(data.isValid()) {
      mMin = std::min(mMin, data.getVal());
      mMax = std::max(mMax, data.getVal());
    }

    mDataColOrganized[col].rows[row] = data;
  }

  void setDataId(uint32_t row, uint32_t col, uint64_t id)
  {
    mDataColOrganized[col].rows[row].setId(id);
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
    return std::max(mNrOfRows, getRowHeaderSize());
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
    return mDataColOrganized.size();
  }

  [[nodiscard]] std::string getTitle() const
  {
    return mTitle;
  }

  [[nodiscard]] const Meta &getMeta() const
  {
    return mMeta;
  }

  void clear();
  bool empty() const
  {
    return getRows() == 0;
  }

  void arrangeByTrackingId();

private:
  /////////////////////////////////////////////////////
  entry_t mDataColOrganized;    // <ROW, <COL, DATA>>
  double mMin = std::numeric_limits<double>::max();
  double mMax = std::numeric_limits<double>::min();

  std::map<uint32_t, std::string> mRowHeader;
  uint32_t mNrOfCols = 0;
  uint32_t mNrOfRows = 0;
  std::string mTitle;
  Meta mMeta;
};

}    // namespace joda::table
