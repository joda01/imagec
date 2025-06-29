///
/// \file      dashboard.cpp
/// \author    Joachim Danmayr
/// \date      2025-06-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dashboard.hpp"
#include <qgridlayout.h>
#include <qwidget.h>
#include <string>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/table/table.hpp"
#include "ui/gui/helper/word_wrap_header.hpp"
#include "ui/gui/results/dashboard/dashboard_element.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
Dashboard::Dashboard()
{
  // setViewMode(QMdiArea::TabbedView);
  setTabsMovable(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::reset()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::tableToQWidgetTable(const joda::table::Table &tableIn)
{
  clearLayout();

  struct Entry
  {
    std::string colName;
    const table::TableColumn *intersectingCol = nullptr;
    std::vector<const table::TableColumn *> cols;
  };

  std::map<enums::ClassId, Entry> dashboards;
  std::map<enums::ClassId, Entry> intersecting;

  for(const auto &[_, col] : tableIn.columns()) {
    if(col.colSettings.measureChannel == enums::Measurement::INTERSECTING) {
      auto &work           = intersecting[col.colSettings.intersectingChannel];
      work.intersectingCol = &col;
      work.colName         = col.colSettings.names.intersectingName;
    }
  }

  for(const auto &[_, col] : tableIn.columns()) {
    std::cout << col.colSettings.createHeader() << std::endl;
    // Put the parents of the intersecting to the
    if(intersecting.contains(col.colSettings.classId)) {
      auto &ed   = intersecting[col.colSettings.classId];
      ed.colName = col.colSettings.names.className;
      ed.cols.emplace_back(&col);
    } else {
      auto &ed   = dashboards[col.colSettings.classId];
      ed.colName = col.colSettings.names.className;
      ed.cols.emplace_back(&col);
    }
  }

  auto createDashboards = [this](const std::map<enums::ClassId, Entry> &entries) {
    for(const auto &[_, dashData] : entries) {
      auto *element01 = new DashboardElement(this);
      element01->setData(dashData.colName.data(), dashData.cols, dashData.intersectingCol);
      element01->show();
    }
  };

  createDashboards(dashboards);
  createDashboards(intersecting);

  tileSubWindows();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::copyToClipboard() const
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::clearLayout()
{
  for(QMdiSubWindow *sub : subWindowList()) {
    sub->close();    // Emits closeEvent, windows get deleted if WA_DeleteOnClose is set
  }
}

/*

  if(mTable->horizontalHeader()->count() > cellX) {
    headerTxt = mTable->horizontalHeaderItem(cellX)->text();
  }
*/

}    // namespace joda::ui::gui
