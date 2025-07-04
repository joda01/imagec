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

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class DashboardElement : public QMdiSubWindow, public exporter::Exportable
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DashboardElement(QWidget *widget);
  void setData(const QString &description, const std::vector<const table::TableColumn *> &cols, bool isImageView, bool isColoc,
               const table::TableColumn *intersectingColl = nullptr);
  void copyTableToClipboard() const;
  void resetData();
  const std::string &getTitle() const override;
  auto getTable() const -> const QTableWidget & override;

signals:
  void cellSelected(joda::table::TableCell);
  void cellDoubleClicked(joda::table::TableCell);

private:
  /////////////////////////////////////////////////////
  void setHeader(const QString &);

  /////////////////////////////////////////////////////
  std::string mTitle;
  QLabel *mHeaderLabel;
  PlaceholderTableWidget *mTable;
  std::map<uint32_t, std::map<uint32_t, const joda::table::TableCell *>> mTableCells;

public slots:
  /////////////////////////////////////////////////////
  void onCellClicked(int row, int column);
};

}    // namespace joda::ui::gui
