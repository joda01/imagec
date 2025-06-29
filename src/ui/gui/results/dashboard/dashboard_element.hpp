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
#include <qtmetamacros.h>
#include <qwidget.h>
#include "backend/helper/table/table.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class DashboardElement : public QMdiSubWindow
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DashboardElement(QWidget *widget);
  void setData(const QString &description, const std::vector<const table::TableColumn *> &cols, const table::TableColumn *intersectingColl = nullptr);
  void copyTableToClipboard() const;
  void reset();

private:
  /////////////////////////////////////////////////////
  void setHeader(const QString &);

  /////////////////////////////////////////////////////
  QLabel *mHeaderLabel;
  PlaceholderTableWidget *mTable;

public slots:
  /////////////////////////////////////////////////////
  void onTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
  void onCellClicked(int row, int column);
};

}    // namespace joda::ui::gui
