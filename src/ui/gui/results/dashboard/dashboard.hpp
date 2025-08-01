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
#include <set>
#include <vector>
#include "backend/database/data/dashboard/data_dashboard.hpp"
#include "backend/database/exporter/exportable.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/table/table.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::table {
class Table;
}    // namespace joda::table

namespace joda::ui::gui {

class DashboardElement;
class WindowMain;
class WindowResults;

class Dashboard : public QMdiArea
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  Dashboard(WindowResults *panelResults);
  void reset();
  void tableToQWidgetTable(const std::shared_ptr<joda::table::Table> tableIn, const std::set<std::set<enums::ClassId>> &classesWithSameTrackingId,
                           bool isImageView);
  void copyToClipboard() const;
  auto getExportables() const -> std::vector<const exporter::Exportable *>;
  void minimizeSubWindows();
  void restoreSubWindows();
  auto getSelectedRows() const -> std::vector<joda::table::TableCell>;
  void setWindowDisplayMode(bool);

private:
  enum class DashboardType
  {
    UNKNOWN,
    NORMAL,
    INTERSECTION,
    DISTANCE,
    COLOC
  };

  /////////////////////////////////////////////////////
  void copyTableToClipboard(QTableWidget *table) const;
  void paintEvent(QPaintEvent *event) override;

  /////////////////////////////////////////////////////
  WindowResults *mWindowResults;
  std::map<joda::db::data::Dashboard::TabWindowKey, QMdiSubWindow *> mMidiWindows;
  bool mFirstOpen = true;
};

}    // namespace joda::ui::gui
