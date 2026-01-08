///
/// \file      table_model.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "table_model_painted_polygon.hpp"
#include <qbrush.h>
#include <qnamespace.h>
#include <qtableview.h>
#include <QFile>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "ui/gui/helper/item_data_roles.hpp"
#include "ui/gui/helper/table_view.hpp"

namespace joda::ui::gui {

TableModelPaintedPolygon::TableModelPaintedPolygon(const joda::settings::Classification *classification,
                                                   const std::shared_ptr<atom::ObjectList> &polygons, QObject *parentWidget) :
    QAbstractTableModel(parentWidget),
    mClassification(classification), mObjectMap(polygons)
{
  if(parentWidget == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
  polygons->registerOnStartChangeCallback([this] {
    auto *tbl      = dynamic_cast<PlaceholderTableView *>(parent());
    auto selection = tbl->selectionModel()->selectedIndexes();
    mSelectedData.clear();
    for(const auto &item : selection) {
      mSelectedData.emplace(data(item, joda::ui::gui::ItemDataRole::UserRoleIdRole).toULongLong());
    }
    beginResetModel();
    //
  });
  polygons->registerOnChangeCallback([this] {
    sortData();
    endResetModel();

    // Only restore selection if the size keeps the same
    auto *tbl                = dynamic_cast<PlaceholderTableView *>(parent());
    QItemSelectionModel *sel = tbl->selectionModel();
    sel->blockSignals(true);

    for(const auto &idx : mSelectedData) {
      const auto foundRow = indexFor(idx);
      if(foundRow < 0) {
        continue;
      }
      QModelIndex newIndex = index(foundRow, 0);
      if(newIndex.isValid()) {
        sel->select(newIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
      }
    }
    sel->blockSignals(false);
  });
}

int TableModelPaintedPolygon::rowCount(const QModelIndex & /*parent*/) const
{
  if(mObjectMap == nullptr) {
    return 0;
  }
  return static_cast<int>(mObjectMap->sizeList());
}

int TableModelPaintedPolygon::columnCount(const QModelIndex & /*parent*/) const
{
  if(mObjectMap == nullptr) {
    return 0;
  }
  return 1;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QVariant TableModelPaintedPolygon::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  if(mObjectMap == nullptr) {
    return {};
  }
  if(role != Qt::DisplayRole) {
    return {};
  }
  if(section == 0) {
    return {"Region of interests"};
  }
  return {};
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QVariant TableModelPaintedPolygon::data(const QModelIndex &index, int role) const
{
  if(mObjectMap == nullptr) {
    return {};
  }

  if(index.row() < 0 || index.row() >= static_cast<int32_t>(mObjectMap->sizeList())) {
    return {};
  }

  const auto *roi = getCell(index.row());
  if(roi == nullptr) {
    return {};
  }
  if(role == joda::ui::gui::ItemDataRole::UserRoleClassId) {
    return static_cast<int32_t>(roi->getClassId());
  }

  if(role == Qt::UserRole) {
    return mClassification->getClassFromId(roi->getClassId()).color.c_str();
  }

  if(role == joda::ui::gui::ItemDataRole::UserRoleElementIsDisabled) {
    return false;
  }

  if(role == joda::ui::gui::ItemDataRole::UserRoleIdRole) {
    return QVariant::fromValue<quint64>(roi->getObjectId());
  }

  if(role == Qt::DisplayRole) {
    QString className = mClassification->getClassFromId(roi->getClassId()).name.c_str();
    if(roi->getCategory() == joda::atom::ROI::Category::MANUAL_SEGMENTATION) {
      if(roi->getClassId() == enums::ClassId::NONE) {
        return "(M) None";
      }
      if(index.column() == 0) {
        return "(M) " + className;
      }
    } else {
      if(index.column() == 0) {
        return "(A) " + className;
      }
    }

    if(index.column() == 1) {
      QString retStr;
      return retStr;
    }
  }
  return {};
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto TableModelPaintedPolygon::getCell(int row) const -> atom::ROI *
{
  if(row >= 0 && row < static_cast<int>(mSortedData.size())) {
    return mSortedData.at(static_cast<size_t>(row)).second;
  }
  return nullptr;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
int32_t TableModelPaintedPolygon::indexFor(atom::ROI *item) const
{
  int32_t idx = 0;
  for(const auto &[_, roi] : mSortedData) {
    if(roi == item) {
      return idx;
    }
    idx++;
  }

  return 0;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
int32_t TableModelPaintedPolygon::indexFor(uint64_t idxIn) const
{
  int32_t idx = 0;
  for(const auto &[idxO, _] : mSortedData) {
    if(idxO == idxIn) {
      return idx;
    }
    idx++;
  }

  return -1;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void TableModelPaintedPolygon::sortData()
{
  mSortedData = std::vector<std::pair<uint64_t, joda::atom::ROI *>>(mObjectMap->getObjectList()->begin(), mObjectMap->getObjectList()->end());

  std::sort(mSortedData.begin(), mSortedData.end(), [](const auto &a, const auto &b) {
    const joda::atom::ROI *ra = a.second;
    const joda::atom::ROI *rb = b.second;

    return (ra->getCategory() > rb->getCategory())   ? true
           : (ra->getCategory() < rb->getCategory()) ? false
           : (ra->getClassId() < rb->getClassId())   ? true
           : (ra->getClassId() > rb->getClassId())   ? false
                                                     : (ra->getObjectId() < rb->getObjectId());
  });
}

}    // namespace joda::ui::gui
