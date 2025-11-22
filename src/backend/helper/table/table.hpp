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
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "backend/database/exporter/exportable.hpp"
#include "backend/enums/bigtypes.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/base32.hpp"
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
    bool isValid                = false;
    uint32_t tStack             = 0;
    uint32_t zStack             = 0;
    uint32_t cStack             = 0;
    std::string rowName         = {};
  };

  struct Grouping
  {
    uint64_t groupIdx = 0;
    uint32_t posX     = 0;
    uint32_t posY     = 0;
  };

  struct Formating
  {
    enum class Color
    {
      BASE_0,
      ALTERNATE_0,
      BASE_1,
      ALTERNATE_1
    };
    Color bgColor         = Color::BASE_0;
    bool isObjectId       = false;
    bool isParentObjectId = false;
    bool isTrackingId     = false;
  };

  /////////////////////////////////////////////////////
  TableCell() = default;

  TableCell(const TableCell &other)
  {
    value       = other.value;
    mMetaData   = other.mMetaData;
    mGrouping   = other.mGrouping;
    mFormatting = other.mFormatting;
  }

  explicit TableCell(const std::shared_ptr<const TableCell> &other)
  {
    value       = other->value;
    mMetaData   = other->mMetaData;
    mGrouping   = other->mGrouping;
    mFormatting = other->mFormatting;
  }

  TableCell(double val, MetaData meta, const Grouping &grouping) : value(val), mMetaData(std::move(meta)), mGrouping(grouping)
  {
  }

  [[nodiscard]] double getVal() const
  {
    return value;
  }
  void setVal(double val)
  {
    value = val;
  }

  [[nodiscard]] std::variant<std::string, double> getValAsVariant(enums::Measurement meas) const
  {
    if(meas == enums::Measurement::OBJECT_ID) {
      return helper::toBase32(getObjectId());
    }
    if(meas == enums::Measurement::PARENT_OBJECT_ID) {
      return helper::toBase32(getParentId());
    }
    if(meas == enums::Measurement::TRACKING_ID) {
      return helper::toBase32(getTrackingId());
    }
    if(meas == enums::Measurement::DISTANCE_TO_OBJECT_ID) {
      return helper::toBase32(getDistanceToObjectId());
    }
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
    return std::isnan(value) || std::isinf(value);
  }

  [[nodiscard]] uint32_t getPosX() const
  {
    return mGrouping.posX;
  }

  [[nodiscard]] uint32_t getPosY() const
  {
    return mGrouping.posY;
  }

  [[nodiscard]] uint64_t getGroupId() const
  {
    return mGrouping.groupIdx;
  }

  [[nodiscard]] uint32_t getStackT() const
  {
    return mMetaData.tStack;
  }

  [[nodiscard]] const std::string &getRowName() const
  {
    return mMetaData.rowName;
  }

  void setRowName(const std::string &rowName)
  {
    mMetaData.rowName = rowName;
  }

  void setBackgroundColor(Formating::Color color)
  {
    mFormatting.bgColor = color;
  }

  void setIsObjectIdCell(bool enable)
  {
    mFormatting.isObjectId = enable;
  }

  void setIsParentObjectIdCell(bool enable)
  {
    mFormatting.isParentObjectId = enable;
  }

  void setIsTrackinIdCell(bool enable)
  {
    mFormatting.isTrackingId = enable;
  }

  [[nodiscard]] auto getFormatting() const -> const Formating &
  {
    return mFormatting;
  }

private:
  /////////////////////////////////////////////////////
  double value = std::numeric_limits<double>::quiet_NaN();
  MetaData mMetaData;
  Grouping mGrouping;
  Formating mFormatting;
};

using colRows_t = std::map<uint32_t, std::shared_ptr<TableCell>>;    // This is a column with its rows

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
class Table : public joda::exporter::Exportable
{
public:
  /////////////////////////////////////////////////////
  void setColHeader(const std::map<uint32_t, settings::ResultsSettings::ColumnKey> &);
  void setColHeader(uint32_t colIdx, const settings::ResultsSettings::ColumnKey &data);

  Table();
  explicit Table(const std::vector<TableColumn> &);
  void setTitle(const std::string &title);
  void init(uint32_t cols, uint32_t rows);

  [[nodiscard]] auto columns() const -> const entry_t &
  {
    return mDataColOrganized;
  }

  auto mutableColumns() -> entry_t &
  {
    return mDataColOrganized;
  }

  [[nodiscard]] std::shared_ptr<const TableCell> data(uint32_t row, uint32_t col) const;
  [[nodiscard]] std::shared_ptr<TableCell> mutableData(uint32_t row, uint32_t col);

  void setData(uint32_t row, uint32_t col, const std::shared_ptr<TableCell> &data)
  {
    mDataColOrganized[col].rows[row] = data;
  }

  void setData(uint32_t row, uint32_t col, const TableCell &data)
  {
    if(data.getRowName().empty()) {
      if(mDataColOrganized[col].rows[row] != nullptr) {
        const auto rowName               = mDataColOrganized[col].rows[row]->getRowName();
        mDataColOrganized[col].rows[row] = std::make_shared<TableCell>(data);
        mDataColOrganized[col].rows[row]->setRowName(rowName);
      } else {
        mDataColOrganized[col].rows[row] = std::make_shared<TableCell>(data);
      }
    } else {
      mDataColOrganized[col].rows[row] = std::make_shared<TableCell>(data);
    }
  }

  void setDataId(uint32_t row, uint32_t col, uint64_t id, const std::string &rowName)
  {
    if(mDataColOrganized[col].rows[row] == nullptr) {
      mDataColOrganized[col].rows[row] = std::make_shared<TableCell>();
      mDataColOrganized[col].rows[row]->setRowName(rowName);
    }
    mDataColOrganized[col].rows[row]->setId(id);
  }
  [[nodiscard]] uint32_t getNrOfRows() const
  {
    if(mDataColOrganized.empty()) {
      return 0;
    }
    uint32_t nr = 0;
    for(const auto &col : mDataColOrganized) {
      if(!col.second.rows.empty()) {
        auto tmp = std::prev(col.second.rows.end())->first + 1;
        if(tmp > nr) {
          nr = tmp;
        }
      }
    }
    return static_cast<uint32_t>(nr);
  }

  [[nodiscard]] uint16_t getNrOfCols() const
  {
    if(mDataColOrganized.empty()) {
      return 0;
    }
    return static_cast<uint16_t>(std::prev(mDataColOrganized.end())->first + 1);
  }

  [[nodiscard]] const settings::ResultsSettings::ColumnKey &getColHeader(uint32_t col) const
  {
    if(!mDataColOrganized.contains(col)) {
      static settings::ResultsSettings::ColumnKey ret;
      return ret;
    }
    return mDataColOrganized.at(col).colSettings;
  }

  [[nodiscard]] const std::string &getColHeaderTitle(uint32_t col) const
  {
    if(!mDataColOrganized.contains(col)) {
      static std::string ret;
      return ret;
    }
    return mDataColOrganized.at(col).title;
  }

  [[nodiscard]] const std::string &getRowHeader(uint32_t row) const
  {
    static std::string ret;
    if(mDataColOrganized.empty()) {
      return ret;
    }
    for(const auto &col : mDataColOrganized) {
      if(col.second.rows.contains(row)) {
        if(!col.second.rows.at(row)->getRowName().empty()) {
          return col.second.rows.at(row)->getRowName();
        }
      }
    }
    return ret;
  }

  [[nodiscard]] const std::string &getTitle() const override
  {
    return mTitle;
  }

  [[nodiscard]] const joda::table::Table &getTable() const override
  {
    return *this;
  }

  void clear();
  void arrangeByTrackingId();
  [[nodiscard]] std::pair<double, double> getMinMax(uint32_t column) const;
  [[nodiscard]] std::pair<double, double> getMinMax() const;

private:
  /////////////////////////////////////////////////////
  entry_t mDataColOrganized;    // <ROW, <COL, DATA>>
  std::string mTitle;
};

}    // namespace joda::table
