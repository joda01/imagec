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
void Dashboard::tableToQWidgetTable(const joda::table::Table &tableIn, const std::set<std::set<enums::ClassId>> &classesWithSameTrackingId,
                                    bool isImageView)
{
  struct Entry
  {
    std::string colName;
    const table::TableColumn *intersectingCol = nullptr;
    std::vector<const table::TableColumn *> cols;
  };

  std::map<std::set<enums::ClassId>, uint32_t> classesWithSameTrackingIdMappingTable;

  std::map<uint32_t, Entry> dashboards;
  std::map<uint32_t, Entry> intersecting;
  std::map<uint32_t, Entry> distance;
  std::map<uint32_t, Entry> colocalizing;

  auto isDistance = [](enums::Measurement measure) {
    return measure == enums::Measurement::DISTANCE_CENTER_TO_CENTER || measure == enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MAX ||
           measure == enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MIN || measure == enums::Measurement::DISTANCE_FROM_OBJECT_ID ||
           measure == enums::Measurement::DISTANCE_TO_OBJECT_ID || measure == enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX ||
           measure == enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN;
  };

  // ========================================
  // First find the intersecting classes
  // ========================================
  uint32_t mapIdIdx = 0;
  for(const auto &[_, col] : tableIn.columns()) {
    if(col.colSettings.measureChannel == enums::Measurement::INTERSECTING) {
      auto &work           = intersecting[static_cast<uint32_t>(col.colSettings.intersectingChannel)];
      work.intersectingCol = &col;
      work.colName         = col.colSettings.names.intersectingName;
    }

    // Group the colocalizing classes (those with at least one common tracking id) to one table
    for(const auto &colcGroup : classesWithSameTrackingId) {
      if(colcGroup.contains(col.colSettings.classId)) {
        uint32_t mapId = 0;
        if(classesWithSameTrackingIdMappingTable.contains(colcGroup)) {
          mapId = classesWithSameTrackingIdMappingTable.at(colcGroup);
        } else {
          classesWithSameTrackingIdMappingTable.emplace(colcGroup, mapIdIdx);
          mapIdIdx++;
        }
        auto &work = colocalizing[mapIdIdx];
        work.cols.emplace_back(&col);
        work.colName = col.colSettings.names.className;
      }
    }
  }

  // ========================================
  // Now create te other dashboard columns
  // ========================================
  for(const auto &[_, col] : tableIn.columns()) {
    // This is a distance measurement. We create a own dashboard for each distance measure if we are in image view
    if(isDistance(col.colSettings.measureChannel) && isImageView) {
      uint32_t key = (static_cast<uint16_t>(col.colSettings.classId) << 16) | static_cast<uint16_t>(col.colSettings.intersectingChannel);
      auto &ed     = distance[key];
      ed.colName   = "Distance " + col.colSettings.names.className + " to " + col.colSettings.names.intersectingName;
      ed.cols.emplace_back(&col);
    } else if(intersecting.contains(static_cast<uint32_t>(col.colSettings.classId))) {
      auto &ed   = intersecting[static_cast<uint32_t>(col.colSettings.classId)];
      ed.colName = col.colSettings.names.className;
      ed.cols.emplace_back(&col);
    } else {
      auto &ed   = dashboards[static_cast<uint32_t>(col.colSettings.classId)];
      ed.colName = col.colSettings.names.className;
      ed.cols.emplace_back(&col);
    }
  }

  // ========================================
  // Lamda function to create the dashboard
  // ========================================
  std::set<MidiWindowKey> availableCols;
  auto createDashboards = [this, &isImageView, &availableCols](const std::map<uint32_t, Entry> &entries, DashboardType dashboardType) {
    for(const auto &[key, dashData] : entries) {
      auto midiKey = MidiWindowKey{dashboardType, key};

      DashboardElement *element01;
      if(mMidiWindows.contains(midiKey)) {
        element01 = mMidiWindows.at(midiKey);
      } else {
        element01 = new DashboardElement(this);
      }
      availableCols.emplace(midiKey);
      mMidiWindows.emplace(midiKey, element01);
      element01->setData(dashData.colName.data(), dashData.cols, isImageView, dashboardType == DashboardType::COLOC, dashData.intersectingCol);
      element01->show();
      connect(element01, &DashboardElement::cellSelected,
              [this](joda::table::TableCell cell) { mPanelResults->setSelectedElement(cell.getPosX(), cell.getPosY(), cell); });
      connect(element01, &DashboardElement::cellDoubleClicked, [this](joda::table::TableCell cell) { mPanelResults->openNextLevel({cell}); });
    }
  };

  createDashboards(dashboards, DashboardType::NORMAL);
  createDashboards(intersecting, DashboardType::INTERSECTION);
  createDashboards(distance, DashboardType::DISTANCE);
  createDashboards(colocalizing, DashboardType::COLOC);

  // ========================================
  // Remove not used midi windows
  // ========================================
  for(auto &[key, window] : mMidiWindows) {
    if(!availableCols.contains(key)) {
      window->close();
      delete window;
      mMidiWindows.erase(key);
    }
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

/*

  if(mTable->horizontalHeader()->count() > cellX) {
    headerTxt = mTable->horizontalHeaderItem(cellX)->text();
  }
*/

}    // namespace joda::ui::gui
