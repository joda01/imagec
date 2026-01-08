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
#include <QFile>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "ui/gui/helper/item_data_roles.hpp"

namespace joda::ui::gui {

TableModelPipeline::TableModelPipeline(const joda::settings::Classification &classSettings, joda::settings::AnalyzeSettings *analyzeSettings,
                                       QObject *parent) :
    QAbstractTableModel(parent),
    mAnalyzeSettings(analyzeSettings), mClassSettings(classSettings)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
  mAnalyzeSettings->registerSettingsChanged([this](const settings::AnalyzeSettings &) { allDataChanged(); });
}

int TableModelPipeline::rowCount(const QModelIndex & /*parent*/) const
{
  if(mAnalyzeSettings == nullptr) {
    return 0;
  }
  return static_cast<int>(mAnalyzeSettings->pipelines.size());
}

int TableModelPipeline::columnCount(const QModelIndex & /*parent*/) const
{
  if(mAnalyzeSettings == nullptr) {
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
QVariant TableModelPipeline::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  if(mAnalyzeSettings == nullptr) {
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
QVariant TableModelPipeline::data(const QModelIndex &index, int role) const
{
  if(mAnalyzeSettings == nullptr) {
    return {};
  }

  if(index.row() < 0 || index.row() >= static_cast<int32_t>(mAnalyzeSettings->pipelines.size())) {
    return {};
  }

  auto it = mAnalyzeSettings->pipelines.begin();
  std::advance(it, index.row());

  if(role == joda::ui::gui::ItemDataRole::UserRoleClassId) {
    return static_cast<int32_t>(it->pipelineSetup.defaultClassId);
  }

  if(role == joda::ui::gui::ItemDataRole::UserRoleElementIsDisabled) {
    return it->disabled;
  }

  if(role == joda::ui::gui::ItemDataRole::UserRoleChannelId) {
    return static_cast<int32_t>(it->pipelineSetup.cStackIndex);
  }

  if(role == Qt::UserRole) {
    return QColor(mClassSettings.getClassFromId(it->pipelineSetup.defaultClassId).color.data());
  }

  if(role == Qt::DisplayRole) {
    QString suffix;
    QString imgChannel = QString::number(it->pipelineSetup.cStackIndex);
    if(it->pipelineSetup.cStackIndex < 0) {
      imgChannel = "None";
    }
    if(index.column() == 0) {
      if(it->disabled) {
        suffix += " (Disabled)";
      }

      QString html = "%1";
      return html.arg(QString(it->meta.name.data()) + suffix);
    }
    if(index.column() == 1) {
      QString retStr;
      retStr = mClassSettings.getClassFromId(it->pipelineSetup.defaultClassId).name.data();
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
auto TableModelPipeline::getCell(int row) -> joda::settings::Pipeline *
{
  if(row >= 0 && row < static_cast<int32_t>(mAnalyzeSettings->pipelines.size())) {
    auto it = mAnalyzeSettings->pipelines.begin();
    std::advance(it, row);
    return &*it;
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
void TableModelPipeline::beginInsertPipeline()
{
  int newRowIndex = rowCount();
  beginInsertRows(QModelIndex(), newRowIndex, newRowIndex);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void TableModelPipeline::endInsertPipeline()
{
  endInsertRows();
}

void TableModelPipeline::resetModel()
{
  beginResetModel();
  endResetModel();
}

void TableModelPipeline::allDataChanged()
{
  QModelIndex indexToUpdtStart = index(0, 0);
  QModelIndex indexToUpdtEnd   = index(rowCount(), 1);
  dataChanged(indexToUpdtStart, indexToUpdtEnd);
}

}    // namespace joda::ui::gui
