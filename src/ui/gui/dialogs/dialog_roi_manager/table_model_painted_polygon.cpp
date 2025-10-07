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

namespace joda::ui::gui {

TableModelPaintedPolygon::TableModelPaintedPolygon(const joda::settings::Classification *classification,
                                                   const std::shared_ptr<atom::ObjectList> &polygons, QObject *parent) :
    QAbstractTableModel(parent),
    mClassification(classification), mObjectMap(polygons)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
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

  const auto *list = mObjectMap->getObjectList();

  auto it = list->begin();
  std::advance(it, index.row());

  if(role == CLASS_ROLE) {
    return static_cast<int32_t>(it->second->getClassId());
  }

  if(role == Qt::UserRole) {
    return mClassification->getClassFromId(it->second->getClassId()).color.c_str();
  }

  if(role == Qt::DisplayRole) {
    if(it->second->getCategory() == joda::atom::ROI::Category::MANUAL_SEGMENTATION) {
      if(it->second->getClassId() == enums::ClassId::NONE) {
        return "None";
      }
      if(index.column() == 0) {
        return "Annotation " + QString::number(static_cast<int32_t>(it->second->getClassId()));
      }
    } else {
      if(index.column() == 0) {
        return "Class " + QString::number(static_cast<int32_t>(it->second->getClassId()));
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
auto TableModelPaintedPolygon::getCell(int row) -> atom::ROI *
{
  const auto *list = mObjectMap->getObjectList();
  if(row >= 0 && row < static_cast<int32_t>(list->size())) {
    auto it = list->begin();
    std::advance(it, row);
    return it->second;
  }
  return nullptr;
}

void TableModelPaintedPolygon::refresh()
{
  beginResetModel();
  endResetModel();
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
  const auto *list = mObjectMap->getObjectList();

  auto it = list->find(item->getObjectId());
  if(it != list->end()) {
    size_t index = std::distance(list->begin(), it);
    return static_cast<int32_t>(index);
  }
  return 0;
}

}    // namespace joda::ui::gui
