///
/// \file      panel_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_heatmap.hpp"
#include <QtSvg/qsvggenerator.h>
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include "../panel_results.hpp"
#include "backend/helper/database/exporter/heatmap/export_heatmap.hpp"
#include "backend/helper/database/exporter/heatmap/export_heatmap_settings.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "ui/gui/container/container_button.hpp"
#include "ui/gui/container/container_label.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ChartHeatMap::ChartHeatMap(PanelResults *parent, joda::settings::ResultsSettings &settings) :
    QWidget(parent), mParent(parent), mHeatmapPainter(settings.mutableDensityMapSettings()), mSettings(settings)
{
  setMinimumSize(parent->size());
  setMouseTracking(true);
}

void ChartHeatMap::setData(const joda::table::Table &data, int32_t newHierarchy)
{
  mSettings.mutableDensityMapSettings().form = static_cast<PanelResults::Navigation>(newHierarchy) == PanelResults::Navigation::PLATE
                                                   ? joda::settings::DensityMapSettings::ElementForm::CIRCLE
                                                   : joda::settings::DensityMapSettings::ElementForm::RECTANGLE;
  mHeatmapPainter.setData(data);
  if(mActHierarchy > newHierarchy) {
    // We navigate back
    mSelection[mActHierarchy].mSelectedWell = -1;
    mHoveredWell                            = -1;
  }
  mActHierarchy = newHierarchy;
  update();
  if(mSelection[mActHierarchy].mSelectedWell >= 0) {
    auto x = mSelection[mActHierarchy].mSelectedPoint.x();
    auto y = mSelection[mActHierarchy].mSelectedPoint.y();
    emit onElementClick(x, y, mHeatmapPainter.getData().data(x, y));
  }
}

///
/// \brief      Painter
/// \author     Joachim Danmayr
///
void ChartHeatMap::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  mHeatmapPainter.paint(painter, size());
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ChartHeatMap::mouseMoveEvent(QMouseEvent *event)
{
  auto [newHoveredWellId, _] = getWellUnderMouse(event);
  if(newHoveredWellId >= 0) {
    if(!mIsHovering) {
      mIsHovering = true;
      // QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    }
  } else {
    if(mIsHovering) {
      mIsHovering = false;
      // QApplication::restoreOverrideCursor();
    }
  }
  // Update hovering index and trigger repaint if necessary
  if(newHoveredWellId >= 0 && mHoveredWell != newHoveredWellId) {
    mHoveredWell = newHoveredWellId;
    update();    // Trigger repaint to reflect hover state change
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ChartHeatMap::mousePressEvent(QMouseEvent *event)
{
  auto [newSelectedWellId, selectedPoint] = getWellUnderMouse(event);
  // Update hovering index and trigger repaint if necessary
  auto selectedData = mHeatmapPainter.getData().data(selectedPoint.x(), selectedPoint.y());
  if(!selectedData.isNAN()) {
    if(newSelectedWellId >= 0 && mSelection[mActHierarchy].mSelectedWell != newSelectedWellId) {
      mSelection[mActHierarchy].mSelectedWell  = newSelectedWellId;
      mSelection[mActHierarchy].mSelectedPoint = selectedPoint;
      mHeatmapPainter.setSelectedIndex(newSelectedWellId);
      update();    // Trigger repaint to reflect hover state change
    }

    emit onElementClick(mSelection[mActHierarchy].mSelectedPoint.x(), mSelection[mActHierarchy].mSelectedPoint.y(), selectedData);
  }

  if(mHeatmapPainter.isLegendMinSectionCLicked(event->pos())) {
    // Text min pos clicked
    mHeatmapPainter.setHeatMapMin(showInputDialog(mHeatmapPainter.getHeatMapMin()));
    update();
  }

  if(mHeatmapPainter.isLegendMaxSectionCLicked(event->pos())) {
    // Text min pos clicked
    mHeatmapPainter.setHeatMapMax(showInputDialog(mHeatmapPainter.getHeatMapMax()));
    update();
  }
}

double ChartHeatMap::showInputDialog(double defaultVal)
{
  QInputDialog inputDialog(mParent);
  inputDialog.setLabelText("Enter value:");
  inputDialog.setInputMode(QInputDialog::DoubleInput);
  inputDialog.setDoubleDecimals(3);    // Allow up to 3 decimal places
  inputDialog.setCancelButtonText("Reset");
  inputDialog.setDoubleMinimum(std::numeric_limits<double>::min());
  inputDialog.setDoubleMaximum(std::numeric_limits<double>::max());
  inputDialog.setDoubleValue(defaultVal);

  auto ret = inputDialog.exec();
  if(QInputDialog::Accepted == ret) {
    mHeatmapPainter.setMinMaxMode(settings::DensityMapSettings::HeatMapRangeMode::MANUAL);
    return inputDialog.doubleValue();
  }
  mHeatmapPainter.setMinMaxMode(settings::DensityMapSettings::HeatMapRangeMode::AUTO);
  return defaultVal;
}

///
/// \brief
/// \authors
/// \param[in]
/// \param[out]
/// \return
///
void ChartHeatMap::mouseDoubleClickEvent(QMouseEvent *event)
{
  auto [newSelectedWellId, selectedPoint] = getWellUnderMouse(event);
  // Update hovering index and trigger repaint if necessary
  auto selectedData = mHeatmapPainter.getData().data(selectedPoint.x(), selectedPoint.y());
  if(!selectedData.isNAN()) {
    if(newSelectedWellId >= 0 && mSelection[mActHierarchy].mSelectedWell != newSelectedWellId) {
      mSelection[mActHierarchy].mSelectedWell  = newSelectedWellId;
      mSelection[mActHierarchy].mSelectedPoint = selectedPoint;
      update();    // Trigger repaint to reflect hover state change
    }
    emit onDoubleClicked(mSelection[mActHierarchy].mSelectedPoint.x(), mSelection[mActHierarchy].mSelectedPoint.y(), selectedData);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::tuple<int32_t, QPoint> ChartHeatMap::getWellUnderMouse(QMouseEvent *event)
{
  return mHeatmapPainter.getWellAtPosition(event->pos(), size());
}

}    // namespace joda::ui::gui
