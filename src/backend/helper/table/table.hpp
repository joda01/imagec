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
    uint64_t objectIdGroup      = 0;    // Elements with the same object ID group should be printed side by side
    uint64_t objectId           = 0;    // The original object id
    uint64_t parentObjectId     = 0;
    uint64_t trackingId         = 0;
    uint64_t distanceToObjectId = 0;
    bool isValid                = true;
    uint32_t tStack             = 0;
    uint32_t zStack             = 0;
    uint32_t cStack             = 0;
    std::string rowName;
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

  [[nodiscard]] uint64_t getDistanceToObjectId() const
  {
    return mMetaData.distanceToObjectId;
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

  const std::string &getRowName() const
  {
    return mMetaData.rowName;
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
  void setColHeader(const std::map<uint32_t, settings::ResultsSettings::ColumnKey> &);

  Table();
  void setTitle(const std::string &title);

  auto columns() const -> const entry_t &
  {
    return mDataColOrganized;
  }

  [[nodiscard]] TableCell data(uint32_t row, uint32_t col) const;

  void setData(uint32_t row, uint32_t col, const TableCell &data)
  {
    mDataColOrganized[col].rows[row] = data;
  }

  void setDataId(uint32_t row, uint32_t col, uint64_t id)
  {
    mDataColOrganized[col].rows[row].setId(id);
  }
  [[nodiscard]] uint32_t getNrOfRows() const
  {
    uint32_t nr = 0;
    for(const auto &col : mDataColOrganized) {
      if(col.second.rows.size() > nr) {
        nr = col.second.rows.size();
      }
    }
    return nr;
  }

  [[nodiscard]] uint32_t getNrOfCols() const
  {
    return mDataColOrganized.size();
  }

  const std::string &getColHeader(int32_t col) const
  {
    return mDataColOrganized.at(col).title;
  }

  const std::string &getRowHeader(int32_t row) const
  {
    return mDataColOrganized.begin()->second.rows.at(row).getRowName();
  }

  const std::string &getTitle() const
  {
    return mTitle;
  }

  void clear();
  void arrangeByTrackingId();
  std::pair<double, double> getMinMax(int column) const;

private:
  /////////////////////////////////////////////////////
  entry_t mDataColOrganized;    // <ROW, <COL, DATA>>
  std::string mTitle;
};

}    // namespace joda::table
