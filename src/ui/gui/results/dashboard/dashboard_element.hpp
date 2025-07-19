///
/// \file      dashboard_element.hpp
/// \author    Joachim Danmayr
/// \date      2025-06-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qmdisubwindow.h>
#include <qtablewidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "backend/database/exporter/exportable.hpp"
#include "backend/helper/table/table.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::table {
class TableCell;
}

namespace joda::ui::gui {
class TableModel;

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class DashboardElement : public QWidget, public exporter::Exportable
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DashboardElement();
  void setData(const std::shared_ptr<joda::table::Table> table);
  void copyTableToClipboard() const;
  const std::string &getTitle() const override;
  auto getTable() const -> const joda::table::Table & override;
  auto getSelectedRows() const -> std::vector<joda::table::TableCell>;

signals:
  void cellSelected(joda::table::TableCell);
  void cellDoubleClicked(joda::table::TableCell);

private:
  /////////////////////////////////////////////////////
  void setHeader(const QString &);
  void saveSelection();
  void restoreSelection();

  // void closeEvent(QCloseEvent *event)
  // {
  //   event->ignore();    // Prevent closing
  // }

  /////////////////////////////////////////////////////
  std::string mTitle;
  QLabel *mHeaderLabel;
  QTableView *mTableView;
  TableModel *mTableModel;
  std::shared_ptr<joda::table::Table> mTable;

  /////////////////////////////////////////////////////
  int savedRow    = -1;
  int savedColumn = -1;
};

}    // namespace joda::ui::gui
