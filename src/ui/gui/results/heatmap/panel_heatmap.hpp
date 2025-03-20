///
/// \file      panel_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qboxlayout.h>
#include <qcolormap.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include "backend/helper/database/exporter/heatmap/export_heatmap.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::ui::gui {

class PanelResults;

///
/// \class      ChartHeatMap
/// \author     Joachim Danmayr
/// \brief      Chart plotting a heatmap
///
class ChartHeatMap : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  ChartHeatMap(PanelResults *parent, joda::settings::ResultsSettings &);
  void setData(const joda::table::Table &, int32_t newHierarchy);

  [[nodiscard]] const table::Table &getData() const
  {
    return mHeatmapPainter.getData();
  }
  void exportToSVG(const QString &filePath) const
  {
    mHeatmapPainter.exportToSVG(filePath);
  }
  void exportToPNG(const QString &filePath) const
  {
    mHeatmapPainter.exportToPNG(filePath);
  }

signals:
  void onElementClick(int cellX, int cellY, table::TableCell value);
  void onDoubleClicked(int cellX, int cellY, table::TableCell value);

private:
  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  std::tuple<int32_t, QPoint> getWellUnderMouse(QMouseEvent *event);
  double showInputDialog(double defaultVal);

  /////////////////////////////////////////////////////
  struct Selection
  {
    int32_t mSelectedWell = -1;
    QPoint mSelectedPoint;
  };
  int32_t mActHierarchy = 0;
  std::map<int32_t, Selection> mSelection;
  int32_t mHoveredWell = -1;

  db::HeatmapExporter mHeatmapPainter;
  PanelResults *mParent;
  bool mIsHovering = false;

  joda::settings::ResultsSettings &mSettings;
};
}    // namespace joda::ui::gui
