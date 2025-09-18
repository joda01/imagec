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
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::ui::gui {

TableModelPaintedPolygon::TableModelPaintedPolygon(QObject *parent) : QAbstractTableModel(parent)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
}

void TableModelPaintedPolygon::setData(std::map<QGraphicsItem *, PaintedRoiProperties> *polygons)
{
  mPolygons = polygons;
}

int TableModelPaintedPolygon::rowCount(const QModelIndex & /*parent*/) const
{
  if(mPolygons == nullptr) {
    return 0;
  }
  return static_cast<int>(mPolygons->size());
}

int TableModelPaintedPolygon::columnCount(const QModelIndex & /*parent*/) const
{
  if(mPolygons == nullptr) {
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
  if(mPolygons == nullptr) {
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
  if(mPolygons == nullptr) {
    return {};
  }

  if(index.row() < 0 || index.row() >= static_cast<int32_t>(mPolygons->size())) {
    return {};
  }

  auto it = mPolygons->begin();
  std::advance(it, index.row());

  if(role == CLASS_ROLE) {
    return static_cast<int32_t>(it->second.classId);
  }

  if(role == Qt::UserRole) {
    return it->second.pixelClassColor;
  }

  if(role == Qt::DisplayRole) {
    if(it->second.source == PaintedRoiProperties::SourceType::Manual) {
      if(it->second.classId == enums::ClassId::NONE) {
        return "None";
      }
      if(index.column() == 0) {
        return "Annotation " + QString::number(static_cast<int32_t>(it->second.classId));
      }
    } else {
      if(index.column() == 0) {
        return "Class " + QString::number(static_cast<int32_t>(it->second.classId));
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
auto TableModelPaintedPolygon::getCell(int row) -> PaintedRoiProperties *
{
  if(row >= 0 && row < static_cast<int32_t>(mPolygons->size())) {
    auto it = mPolygons->begin();
    std::advance(it, row);
    return &it->second;
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
int32_t TableModelPaintedPolygon::indexFor(QGraphicsItem *item) const
{
  auto it = mPolygons->find(item);
  if(it != mPolygons->end()) {
    size_t index = std::distance(mPolygons->begin(), it);
    return static_cast<int32_t>(index);
  }
  return 0;
}

}    // namespace joda::ui::gui
