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
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::ui::gui {

TableModelPipeline::TableModelPipeline(const joda::settings::Classification &classSettings, QObject *parent) :
    QAbstractTableModel(parent), mClassSettings(classSettings)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }

  auto loadSvg = [](const QString &name) -> QString {
    QFile file(":/icons-svg/22/icons-svg/22/" + name + ".svg");    // or use a file path
    if(file.open(QIODevice::ReadOnly)) {
      QByteArray imageData = file.readAll();
      return QString::fromLatin1(imageData.toBase64());
    }
    return "";
  };

  base64IconName = loadSvg("text-field");
  base64IconHash = loadSvg("irc-operator");
}

void TableModelPipeline::setData(std::list<joda::settings::Pipeline> *pipelines)
{
  mPipelines = pipelines;
}

int TableModelPipeline::rowCount(const QModelIndex & /*parent*/) const
{
  if(mPipelines == nullptr) {
    return 0;
  }
  return static_cast<int>(mPipelines->size());
}

int TableModelPipeline::columnCount(const QModelIndex & /*parent*/) const
{
  if(mPipelines == nullptr) {
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
  if(mPipelines == nullptr) {
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
  const auto ALTERNATE = ((QTableView *) parent())->palette().color(QPalette::AlternateBase);
  const auto BASE      = ((QTableView *) parent())->palette().color(QPalette::Base);

  if(mPipelines == nullptr) {
    return {};
  }

  if(index.row() < 0 || index.row() >= mPipelines->size()) {
    return {};
  }

  auto it = mPipelines->begin();
  std::advance(it, index.row());

  if(role == CLASS_ROLE) {
    return static_cast<int32_t>(it->pipelineSetup.defaultClassId);
  }

  if(role == CHANNEL_IDX_ROLE) {
    return static_cast<int32_t>(it->pipelineSetup.cStackIndex);
  }

  if(role == Qt::UserRole) {
    return QColor(mClassSettings.getClassFromId(it->pipelineSetup.defaultClassId).color.data());
  }

  if(role == Qt::DisplayRole) {
    QString imgChannel = QString::number(it->pipelineSetup.cStackIndex);
    if(it->pipelineSetup.cStackIndex < 0) {
      imgChannel = "None";
    }
    if(index.column() == 0) {
      QString html = "%1";
      return html.arg(QString(it->meta.name.data()));
    }
    if(index.column() == 1) {
      QString retStr;
      if(static_cast<int32_t>(it->pipelineSetup.defaultClassId) >= 0) {
        retStr = mClassSettings.getClassFromId(it->pipelineSetup.defaultClassId).name.data();
      } else {
        retStr = "None";
      }

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
