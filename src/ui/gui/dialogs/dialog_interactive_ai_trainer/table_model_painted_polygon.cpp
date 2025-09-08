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

void TableModelPaintedPolygon::setData(std::vector<PaintedRoiProperties> *polygons)
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
  return 2;
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
    return {"Name"};
  }
  if(section == 1) {
    return {"Class/Channel"};
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
    return static_cast<int32_t>(it->pixelClass);
  }

  if(role == Qt::UserRole) {
    return it->pixelClassColor;
  }

  if(role == Qt::DisplayRole) {
    QString imgChannel = QString::number(it->pixelClass);
    if(it->pixelClass == 0) {
      imgChannel = "Background";
    }
    if(index.column() == 0) {
      QString html = "%1";
      return html.arg(QString::number(it->pixelClass));
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
    return &*it;
  }
  return nullptr;
}

void TableModelPaintedPolygon::refresh()
{
  beginResetModel();
  endResetModel();
}

}    // namespace joda::ui::gui
