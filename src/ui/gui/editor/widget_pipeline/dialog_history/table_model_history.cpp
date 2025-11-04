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

#include "table_model_history.hpp"
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
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

TableModelHistory::TableModelHistory(settings::Pipeline *dataHistory, QObject *parent) : QAbstractTableModel(parent), mDataHistory(dataHistory)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
}

int TableModelHistory::rowCount(const QModelIndex & /*parent*/) const
{
  if(mDataHistory == nullptr) {
    return 0;
  }
  return static_cast<int>(mDataHistory->history.size());
}

int TableModelHistory::columnCount(const QModelIndex & /*parent*/) const
{
  if(mDataHistory == nullptr) {
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
QVariant TableModelHistory::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  if(mDataHistory == nullptr) {
    return {};
  }
  if(role != Qt::DisplayRole) {
    return {};
  }
  if(section == 0) {
    return {"Name"};
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
QVariant TableModelHistory::data(const QModelIndex &index, int role) const
{
  if(mDataHistory == nullptr) {
    return {};
  }

  if(index.row() < 0 || index.row() >= static_cast<int32_t>(mDataHistory->history.size())) {
    return {};
  }

  auto it = mDataHistory->history.begin();
  std::advance(it, index.row());

  if(role == Qt::UserRole) {
    //  return QColor(mClassSettings.getClassFromId(it->mDataHistory.defaultClassId).color.data());
  }

  if(role == Qt::DisplayRole) {
    if(index.column() == 0) {
      QString retStr;
      retStr = it->commitMessage.data();
      return retStr;
    }
  } else if(role == Qt::DecorationRole) {
    if(mDataHistory->getHistoryIndex() == static_cast<size_t>(index.row())) {
      if(!it->tagMessage.empty()) {
        return generateSvgIcon<Style::DUETONE, Color::RED>("tag-simple");
      }
      switch(it->category) {
        case enums::HistoryCategory::ADDED:
          return generateSvgIcon<Style::REGULAR, Color::RED>("list-plus");
        case enums::HistoryCategory::DELETED:
          return generateSvgIcon<Style::REGULAR, Color::RED>("trash-simple");
        case enums::HistoryCategory::CHANGED:
          return generateSvgIcon<Style::REGULAR, Color::RED>("circle");
        case enums::HistoryCategory::SAVED:
          return generateSvgIcon<Style::REGULAR, Color::RED>("floppy-disk");
        default:
          return generateSvgIcon<Style::REGULAR, Color::RED>("circle");
      }
    } else {
      if(!it->tagMessage.empty()) {
        return generateSvgIcon<Style::DUETONE, Color::GREEN>("tag-simple");
      }
      switch(it->category) {
        case enums::HistoryCategory::ADDED:
          return generateSvgIcon<Style::REGULAR, Color::BLACK>("list-plus");
        case enums::HistoryCategory::DELETED:
          return generateSvgIcon<Style::REGULAR, Color::BLACK>("trash-simple");
        case enums::HistoryCategory::CHANGED:
          return generateSvgIcon<Style::REGULAR, Color::BLACK>("circle");
        case enums::HistoryCategory::SAVED:
          return generateSvgIcon<Style::REGULAR, Color::BLACK>("floppy-disk");
        default:
          return generateSvgIcon<Style::REGULAR, Color::BLACK>("circle");
      }
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
auto TableModelHistory::getCell(int row) -> joda::settings::PipelineHistoryEntry *
{
  if(row >= 0 && row < static_cast<int32_t>(mDataHistory->history.size())) {
    auto it = mDataHistory->history.begin();
    std::advance(it, row);
    return &*it;
  }
  return nullptr;
}

void TableModelHistory::refresh()
{
  beginResetModel();
  endResetModel();
}

}    // namespace joda::ui::gui
