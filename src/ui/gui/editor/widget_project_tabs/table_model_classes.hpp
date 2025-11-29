///
/// \file      table_model.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qabstractitemmodel.h>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_classification.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class TableModelClasses : public QAbstractTableModel
{
  Q_OBJECT

public:
  static constexpr int32_t CLASS_ROLE        = 0x101;
  static inline const std::string NONE_COLOR = "#565656";

  /////////////////////////////////////////////////////
  TableModelClasses(joda::settings::Classification *classification, const std::shared_ptr<atom::ObjectList> &polygons, QObject *parent = nullptr);
  auto getCell(int row) const -> const joda::settings::Class;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void beginChange()
  {
    beginResetModel();
  }

  void endChange()
  {
    endResetModel();
  }

  void beginInsertRow()
  {
    int newRowIndex = rowCount();
    beginInsertRows(QModelIndex(), newRowIndex, newRowIndex);
  }

  void endInsertRow()
  {
    endInsertRows();
  }

  void hideElement(int32_t row, bool hide);
  bool isHidden(int32_t row) const;

private:
  /////////////////////////////////////////////////////
  int32_t indexFor(joda::settings::Class *) const;

  joda::settings::Classification *mClassification = nullptr;
  std::shared_ptr<atom::ObjectList> mObjectMap    = nullptr;
  settings::Class mNoneClass                      = joda::settings::Class{.classId = enums::ClassId::NONE, .name = "None", .color = NONE_COLOR};
};

}    // namespace joda::ui::gui
