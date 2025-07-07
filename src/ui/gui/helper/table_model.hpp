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

#include <qabstractitemmodel.h>
#include "backend/helper/table/table.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class TableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  TableModel(QObject *parent = nullptr);
  void setData(const std::shared_ptr<joda::table::Table> table);
  auto getCell(int row, int col) -> const std::shared_ptr<joda::table::TableCell>;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
  std::shared_ptr<joda::table::Table> mTable;
};

}    // namespace joda::ui::gui
