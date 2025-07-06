///
/// \file      graph_qt_backend.cpp
/// \author    Joachim Danmayr
/// \date      2025-06-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "graph_qt_backend.hpp"
#include <qevent.h>
#include <QFile>
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include "backend/helper/image/image.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/plot/heatmap/plot_heatmap.hpp"

namespace joda::ui::gui {

QtBackend::QtBackend(QWidget *parent) : QWidget(parent)
{
  setMouseTracking(true);    // <--- This line is crucial
  mHeatmap = std::make_unique<joda::plot::Heatmap>();
  mHeatmap->setPlotLabels(true);
  mHeatmap->setGapsBetweenBoxes(4);
  mHeatmap->setPrecision(0);
  auto bgColor = palette().color(backgroundRole());
  mHeatmap->setBackgroundColor(cv::Vec3b{(uint8_t) bgColor.blue(), (uint8_t) bgColor.green(), (uint8_t) bgColor.red()});
}

QtBackend::~QtBackend()
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void QtBackend::updateGraph(const joda::table::Table &&data)
{
  mHeatmap->setData(std::move(data));
  update();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void QtBackend::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);
  std::lock_guard<std::mutex> lock(mPaintMutex);
  // auto sizeToDraw = std::min(width(), height());

  auto graph = mHeatmap->plot({width(), height()});
  QPainter painter(this);
  auto pixmap = QPixmap::fromImage(QImage(graph.data, graph.cols, graph.rows, static_cast<uint32_t>(graph.step), QImage::Format_RGB888).rgbSwapped());
  int x       = (width() - pixmap.width()) / 2;
  int y       = (height() - pixmap.height()) / 2;

  mPixmapTopLeft = QPoint(x, y);
  mPixmapSize.setWidth(pixmap.width());
  mPixmapSize.setHeight(pixmap.height());
  painter.drawPixmap(x, y, pixmap);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void QtBackend::mousePressEvent(QMouseEvent *event)
{
  QWidget::mouseMoveEvent(event);
  QPoint click    = event->pos();
  QPoint relative = click - mPixmapTopLeft;
  if(relative.x() >= 0 && relative.y() >= 0 && relative.x() < mPixmapSize.width() && relative.y() < mPixmapSize.height()) {
    auto clicked = mHeatmap->getCellFromCoordinates(relative.x(), relative.y());
    if(clicked.has_value()) {
      const auto &[pos, cell] = clicked.value();
      mHeatmap->setHighlightCell(pos);
      emit onGraphClicked(cell);
    } else {
      mHeatmap->resetHighlight();
    }
  } else {
    mHeatmap->resetHighlight();
  }
  update();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void QtBackend::mouseDoubleClickEvent(QMouseEvent *event)
{
  QWidget::mouseDoubleClickEvent(event);
  QPoint click    = event->pos();
  QPoint relative = click - mPixmapTopLeft;
  if(relative.x() >= 0 && relative.y() >= 0 && relative.x() < mPixmapSize.width() && relative.y() < mPixmapSize.height()) {
    auto clicked = mHeatmap->getCellFromCoordinates(relative.x(), relative.y());
    if(clicked.has_value()) {
      const auto &[pos, cell] = clicked.value();
      mHeatmap->setHighlightCell(pos);
      emit onGraphDoubleClicked(cell);
    } else {
      mHeatmap->resetHighlight();
    }
  } else {
    mHeatmap->resetHighlight();
  }
  update();
}

}    // namespace joda::ui::gui
