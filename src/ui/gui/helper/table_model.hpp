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
#include <mutex>
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
  auto getCell(int row, int col) -> const std::shared_ptr<const joda::table::TableCell>;
  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
  std::shared_ptr<joda::table::Table> mTable;
  std::mutex mChangeMutex;
};

}    // namespace joda::ui::gui
