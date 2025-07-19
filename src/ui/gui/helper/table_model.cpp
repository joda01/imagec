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

#include "table_model.hpp"
#include <qbrush.h>
#include <qnamespace.h>
#include <qtableview.h>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::ui::gui {

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
}

int TableModel::rowCount(const QModelIndex &parent) const
{
  if(!mTable) {
    return 0;
  }
  return mTable->getNrOfRows();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
  if(!mTable) {
    return 0;
  }
  return mTable->getNrOfCols();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(!mTable) {
    return {};
  }
  if(role != Qt::DisplayRole) {
    return {};
  }

  if(orientation == Qt::Orientation::Horizontal) {
    return QString(mTable->getColHeader(section).createHtmlHeader(joda::settings::ResultsSettings::ColumnKey::HeaderStyle::FULL).data());
  }
  if(orientation == Qt::Orientation::Vertical) {
    return QString(mTable->getRowHeader(section).data());
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
QVariant TableModel::data(const QModelIndex &index, int role) const
{
  const auto LIGHT_BLUE = QColor("#ADD8E6");
  const auto DARK_BLUE  = QColor("#87CEEB");
  const auto ALTERNATE  = ((QTableView *) parent())->palette().color(QPalette::AlternateBase);
  const auto BASE       = ((QTableView *) parent())->palette().color(QPalette::Base);

  if(!mTable) {
    return {};
  }
  const auto &cell = mTable->data(index.row(), index.column());
  if(cell == nullptr) {
    return {};
  }

  if(role == Qt::BackgroundRole) {
    switch(cell->getFormatting().bgColor) {
      default:
      case table::TableCell::Formating::Color::BASE_0:
        return QBrush(BASE);    // Custom color based on data
      case table::TableCell::Formating::Color::ALTERNATE_0:
        return QBrush(ALTERNATE);    // Custom color based on data
      case table::TableCell::Formating::Color::BASE_1:
        return QBrush(LIGHT_BLUE);    // Custom color based on data
      case table::TableCell::Formating::Color::ALTERNATE_1:
        return QBrush(DARK_BLUE);    // Custom color based on data
    }
  }

  if(!index.isValid() || role != Qt::DisplayRole) {
    return {};
  }

  auto generateMetaFooter = [](const std::shared_ptr<const joda::table::TableCell> &rowData) -> QString {
    return "";
    /*return "<br><span style=\"color:rgb(155, 153, 153);\"><i>🗝: " + QString(joda::helper::toBase32(rowData.getObjectId()).data()) + " ⬆ " +
           QString(joda::helper::toBase32(rowData.getParentId()).data()) + "<br> ↔ " +
           QString(joda::helper::toBase32(rowData.getDistanceToObjectId()).data()) + " ⚯ " +
           QString(joda::helper::toBase32(rowData.getTrackingId()).data()) + "</i><span>";*/
  };

  const auto formatting = cell->getFormatting();

  if(formatting.isObjectId) {
    return QString(joda::helper::toBase32(cell->getObjectId()).data()) + generateMetaFooter(cell);
  }
  if(formatting.isParentObjectId) {
    return QString(joda::helper::toBase32(cell->getParentId()).data()) + generateMetaFooter(cell);
  }
  if(formatting.isTrackingId) {
    return QString(joda::helper::toBase32(cell->getTrackingId()).data()) + generateMetaFooter(cell);
  }

  return QString::number(cell->getVal()) + generateMetaFooter(cell);
}

void TableModel::setData(const std::shared_ptr<joda::table::Table> table)
{
  std::lock_guard<std::mutex> lock(mChangeMutex);
  beginResetModel();
  // reload or rebind your internal Table data here
  mTable = table;
  endResetModel();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto TableModel::getCell(int row, int col) -> const std::shared_ptr<const joda::table::TableCell>
{
  return mTable->data(row, col);
}

}    // namespace joda::ui::gui
