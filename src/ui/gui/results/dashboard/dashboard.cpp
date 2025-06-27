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
    std::map<uint32_t, const table::TableColumn *> cols;
  };

  std::map<enums::ClassId, Entry> dashboards;

  for(const auto &[key, col] : tableIn.columns()) {
    auto &ed   = dashboards[col.colSettings.classId];
    ed.colName = col.colSettings.names.className;
    ed.cols.emplace(key, &col);
  }

  int32_t row = 0;
  for(const auto &[_, dashData] : dashboards) {
    auto *element01 = new DashboardElement(this);
    element01->setData(dashData.colName.data(), dashData.cols);
    element01->show();
    row++;
  }

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
