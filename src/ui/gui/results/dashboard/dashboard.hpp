///
/// \file      dashboard.hpp
/// \author    Joachim Danmayr
/// \date      2025-06-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qmdiarea.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "ui/gui/helper/table_widget.hpp"

namespace joda::table {
class Table;
}    // namespace joda::table

namespace joda::ui::gui {
class Dashboard : public QMdiArea
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  Dashboard();
  void reset();
  void tableToQWidgetTable(const joda::table::Table &tableIn);
  void copyToClipboard() const;

private:
  /////////////////////////////////////////////////////
  void copyTableToClipboard(QTableWidget *table) const;
  void clearLayout();

  /////////////////////////////////////////////////////
  int32_t mSelectedTableColumnIdx = -1;
  int32_t mSelectedTableRow       = -1;
};

}    // namespace joda::ui::gui
