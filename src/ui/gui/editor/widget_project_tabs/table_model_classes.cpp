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

#include "table_model_classes.hpp"
#include <qbrush.h>
#include <qnamespace.h>
#include <qtableview.h>
#include <QFile>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "ui/gui/helper/item_data_roles.hpp"

namespace joda::ui::gui {

TableModelClasses::TableModelClasses(joda::settings::Classification *classification, const std::shared_ptr<atom::ObjectList> &polygons,
                                     QObject *parent) :
    QAbstractTableModel(parent),
    mClassification(classification), mObjectMap(polygons)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
  if(nullptr != polygons) {
    polygons->registerOnChangeCallback([this] { emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1)); });
  }
}

int TableModelClasses::rowCount(const QModelIndex & /*parent*/) const
{
  if(mClassification == nullptr) {
    return 0;
  }
  return static_cast<int>(mClassification->classes.size()) + 1;    // +1 because we add a None class always at first element
}

int TableModelClasses::columnCount(const QModelIndex & /*parent*/) const
{
  if(mClassification == nullptr) {
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
QVariant TableModelClasses::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  if(mClassification == nullptr) {
    return {};
  }
  if(role != Qt::DisplayRole) {
    return {};
  }
  if(section == 0) {
    return {"Class"};
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
QVariant TableModelClasses::data(const QModelIndex &index, int role) const
{
  if(mClassification == nullptr) {
    return {};
  }

  if(index.row() < 0 || index.row() >= static_cast<int32_t>(mClassification->classes.size()) + 1) {    // +1 because we inject None at the top
    return {};
  }

  auto classs = getCell(index.row());

  if(role == joda::ui::gui::ItemDataRole::UserRoleClassId) {
    return static_cast<int32_t>(classs.classId);
  }

  if(role == Qt::UserRole) {
    return classs.color.c_str();
  }

  if(role == joda::ui::gui::ItemDataRole::UserRoleElementIsDisabled) {
    return classs.hidden;
  }

  if(role == Qt::DisplayRole) {
    QString suffix;
    if(mObjectMap != nullptr) {
      if(mObjectMap->contains(classs.classId)) {
        suffix = " (" + QString::number(mObjectMap->at(classs.classId)->size()) + ")";
      }
      if(classs.hidden) {
        suffix += " (hidden)";
      }
    }
    QString className = QString(classs.name.c_str()) + suffix;
    return className;
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
auto TableModelClasses::getCell(int row) const -> const joda::settings::Class
{
  if(row == 0) {
    return mNoneClass;
  }
  row--;
  if(row >= static_cast<int>(mClassification->classes.size())) {
    return {};
  }
  std::list<joda::settings::Class>::const_iterator it = mClassification->classes.begin();
  std::advance(it, row);
  return *it;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void TableModelClasses::hideElement(int32_t row, bool hide)
{
  QModelIndex indexToUpdt = index(row, 0);

  if(row == 0) {
    mNoneClass.hidden = true;
    endChange();
    return;
  }
  row--;
  auto it    = std::next(mClassification->classes.begin(), row);
  it->hidden = hide;

  dataChanged(indexToUpdt, indexToUpdt);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
bool TableModelClasses::isHidden(int32_t row) const
{
  if(row == 0) {
    return mNoneClass.hidden;
  }
  row--;

  auto it = std::next(mClassification->classes.begin(), row);
  return it->hidden;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
int32_t TableModelClasses::indexFor(joda::settings::Class *item) const
{
  int32_t idx = 0;
  for(const joda::settings::Class &itemIter : mClassification->classes) {
    if(itemIter.classId == item->classId) {
      return idx;
    }
    idx++;
  }

  return 0;
}

}    // namespace joda::ui::gui
