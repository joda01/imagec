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
#include "backend/database/data/dashboard/data_dashboard.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/table/table.hpp"
#include "ui/gui/helper/word_wrap_header.hpp"
#include "ui/gui/results/dashboard/dashboard_element.hpp"
#include "ui/gui/results/panel_results.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
Dashboard::Dashboard(PanelResults *panelResults, WindowMain *mainWindow) : mPanelResults(panelResults), mMainWindow(mainWindow)
{
  // setViewMode(QMdiArea::TabbedView);
  setTabsMovable(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::tableToQWidgetTable(const std::shared_ptr<joda::table::Table> tableIn,
                                    const std::set<std::set<enums::ClassId>> &classesWithSameTrackingId, bool isImageView)
{
  joda::db::data::Dashboard dashboard;
  auto tabs = dashboard.convert(tableIn, classesWithSameTrackingId, isImageView);

  // ========================================
  // Lamda function to create the dashboard
  // ========================================
  std::set<joda::db::data::Dashboard::TabWindowKey> availableCols;
  auto createDashboards = [this, &isImageView, &availableCols](joda::db::data::Dashboard::TabWindowKey midiKey,
                                                               const std::shared_ptr<joda::table::Table> &table) {
    DashboardElement *element01;
    if(mMidiWindows.contains(midiKey)) {
      element01 = mMidiWindows.at(midiKey);
      if(element01->isHidden()) {
        element01->show();
      }
    } else {
      element01 = new DashboardElement(this);
      mMidiWindows.emplace(midiKey, element01);
      connect(element01, &DashboardElement::cellSelected, [this](joda::table::TableCell cell) { mPanelResults->setSelectedElement(cell); });
      connect(element01, &DashboardElement::cellDoubleClicked, [this](joda::table::TableCell cell) { mPanelResults->openNextLevel({cell}); });
      element01->show();
      element01->adjustSize();
    }
    availableCols.emplace(midiKey);
    element01->setData(table);
  };

  QMdiSubWindow *focusedWindow = activeSubWindow();
  for(const auto &[key, table] : tabs) {
    createDashboards(key, table);
  }

  setActiveSubWindow(focusedWindow);    // Restore focus

  // ========================================
  // Remove not used midi windows
  // ========================================
  for(auto &[key, window] : mMidiWindows) {
    if(!availableCols.contains(key)) {
      window->hide();
    }
  }

  if(mFirstOpen) {
    mFirstOpen = false;
    cascadeSubWindows();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::minimizeSubWindows()
{
  QList<QMdiSubWindow *> subwindows = subWindowList();
  for(QMdiSubWindow *subWin : subwindows) {
    subWin->showMinimized();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::restoreSubWindows()
{
  QList<QMdiSubWindow *> subwindows = subWindowList();
  for(QMdiSubWindow *subWin : subwindows) {
    subWin->showNormal();
  }
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
  for(const auto &[_, subWindow] : mMidiWindows) {
    if(subWindow == activeSubWindow()) {
      subWindow->copyTableToClipboard();
      mMainWindow->statusBar()->showMessage("Copied >" + subWindow->windowTitle() + "< data to clipboard", 5000);
    }
  }
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

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Dashboard::getExportables() const -> std::vector<const exporter::Exportable *>
{
  std::vector<const exporter::Exportable *> retVal;

  for(const auto &[_, dashb] : mMidiWindows) {
    retVal.emplace_back(dashb);
  }
  return retVal;
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
  mMidiWindows.clear();
  mFirstOpen = true;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::paintEvent(QPaintEvent *event)
{
  QMdiArea::paintEvent(event);

  // Check if all subwindows are minimized
  bool allMinimized   = true;
  const auto &windows = subWindowList();
  if(windows.isEmpty()) {
    return;
  }

  for(QMdiSubWindow *sub : windows) {
    if(!(sub->windowState() & Qt::WindowMinimized)) {
      allMinimized = false;
      break;
    }
  }

  if(allMinimized) {
    QPainter painter(this);
    QString message = "Open a result window by clicking on the tab bar of a window.";
    QFont font      = painter.font();
    font.setPointSize(14);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(Qt::gray);

    QRect rect = this->viewport()->rect();    // Use viewport to avoid scrollbars
    painter.drawText(rect, Qt::AlignCenter, message);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Dashboard::getSelectedRows() const -> std::vector<joda::table::TableCell>
{
  return ((DashboardElement *) activeSubWindow())->getSelectedRows();
}

}    // namespace joda::ui::gui
