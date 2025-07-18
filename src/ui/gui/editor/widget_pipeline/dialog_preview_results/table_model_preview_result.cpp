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

#include "table_model_preview_result.hpp"
#include <qbrush.h>
#include <qnamespace.h>
#include <qpalette.h>
#include <qtableview.h>
#include <qvariant.h>
#include <QFile>
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::ui::gui {

TableModelPreviewResult::TableModelPreviewResult(const joda::settings::Classification &classSettings, QObject *parent) :
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

void TableModelPreviewResult::setData(joda::ctrl::Preview::PreviewResults *results)
{
  mPreviewResult = results;
  refresh();
}

int TableModelPreviewResult::rowCount(const QModelIndex &parent) const
{
  if(mPreviewResult == nullptr) {
    return 0;
  }
  return mPreviewResult->foundObjects.size();
}

int TableModelPreviewResult::columnCount(const QModelIndex &parent) const
{
  if(mPreviewResult == nullptr) {
    return 0;
  }
  return 1;
}

void TableModelPreviewResult::refresh()
{
  beginResetModel();
  endResetModel();
}

void TableModelPreviewResult::setHiddenFlag(enums::ClassId classs, bool isHidden)
{
  if(nullptr != mPreviewResult) {
    mPreviewResult->foundObjects.at(classs).isHidden = isHidden;
  }
  refresh();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QVariant TableModelPreviewResult::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(mPreviewResult == nullptr) {
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
QVariant TableModelPreviewResult::data(const QModelIndex &index, int role) const
{
  if(mPreviewResult == nullptr) {
    return {};
  }

  if(index.row() < 0 || index.row() >= mPreviewResult->foundObjects.size()) {
    return {};
  }

  auto it = mPreviewResult->foundObjects.begin();
  std::advance(it, index.row());

  if(role == Qt::UserRole) {
    return QColor(it->second.color.data());
  }

  if(role == CLASS_ROLE) {
    return static_cast<int32_t>(it->first);
  }

  if(role == Qt::CheckStateRole) {
    return {it->second.isHidden};
  }

  if(role == Qt::DisplayRole) {
    return QString(mClassSettings.getClassFromId(it->first).name.data()) + " (" + QString::number(it->second.count) + ")";
  }
  return {};
}

}    // namespace joda::ui::gui
