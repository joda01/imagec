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

#include "table_model_pipeline.hpp"
#include <qbrush.h>
#include <qnamespace.h>
#include <qtableview.h>
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::ui::gui {

TableModelPipeline::TableModelPipeline(QObject *parent) : QAbstractTableModel(parent)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
}

void TableModelPipeline::setData(std::list<joda::settings::Pipeline> *pipelines)
{
  mPipelines = pipelines;
}

int TableModelPipeline::rowCount(const QModelIndex &parent) const
{
  if(mPipelines == nullptr) {
    return 0;
  }
  return mPipelines->size();
}

int TableModelPipeline::columnCount(const QModelIndex &parent) const
{
  if(!mPipelines) {
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
QVariant TableModelPipeline::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(mPipelines == nullptr) {
    return {};
  }
  if(role != Qt::DisplayRole) {
    return {};
  }
  return {"Pipelines"};
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QVariant TableModelPipeline::data(const QModelIndex &index, int role) const
{
  const auto ALTERNATE = ((QTableView *) parent())->palette().color(QPalette::AlternateBase);
  const auto BASE      = ((QTableView *) parent())->palette().color(QPalette::Base);

  if(mPipelines == nullptr) {
    return {};
  }

  if(index.row() < 0 || index.row() >= mPipelines->size()) {
    return {};
  }

  if(role == Qt::DisplayRole) {
    auto it = mPipelines->begin();
    std::advance(it, index.row());
    return QString(it->meta.name.data());
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
auto TableModelPipeline::getCell(int row) -> joda::settings::Pipeline *
{
  if(row >= 0 && row < mPipelines->size()) {
    auto it = mPipelines->begin();
    std::advance(it, row);
    return &*it;
  }
  return nullptr;
}

void TableModelPipeline::refresh()
{
  beginResetModel();
  endResetModel();
}

}    // namespace joda::ui::gui
