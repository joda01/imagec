///
/// \file      heatmap_widget.cpp
/// \author    Joachim Danmayr
/// \date      2025-06-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "heatmap_widget.hpp"
#include <qapplication.h>
#include <qclipboard.h>
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

HeatmapWidget::HeatmapWidget(QWidget *parent) : QWidget(parent)
{
  setMouseTracking(true);    // <--- This line is crucial
  mHeatmap = std::make_unique<joda::plot::Heatmap>();
  mHeatmap->setPlotLabels(true);
  mHeatmap->setGapsBetweenBoxes(4);
  mHeatmap->setPrecision(0);
  auto bgColor = palette().color(backgroundRole());
  mHeatmap->setBackgroundColor(cv::Vec3b{(uint8_t) bgColor.blue(), (uint8_t) bgColor.green(), (uint8_t) bgColor.red()});
}

HeatmapWidget::~HeatmapWidget()
{
}
///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapWidget::exportToPNG(const std::filesystem::path &path) const
{
  auto withTmp   = 2048;
  auto heightTmp = 2048;
  auto graph     = mHeatmap->plot({withTmp, heightTmp});
  bool success   = cv::imwrite(path.string(), graph);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapWidget::copyToClipboard() const
{
  const auto &table = mHeatmap->getData();
  QStringList data;
  QStringList header;
  for(int row = 0; row < table.getNrOfRows(); row++) {
    QStringList rowData;
    for(int col = 0; col < table.getNrOfCols(); col++) {
      if(row == 0) {
        header << table.getColHeaderTitle(col).data();
      }
      if(col == 0) {
        rowData << table.getRowHeader(row).data();
      }
      const auto tmp = table.data(row, col);
      if(tmp != nullptr) {
        auto val = tmp->getValAsVariant(table.getColHeader(col).measureChannel);
        QString txtTemp;
        if(tmp->isNAN()) {
          txtTemp = "";
        } else if(std::holds_alternative<std::string>(val)) {
          txtTemp = std::get<std::string>(val).data();
        } else {
          txtTemp = QString::number(std::get<double>(val));
        }
        rowData << txtTemp;
      } else {
        rowData << "";
      }
    }
    data << rowData.join("\t");    // Join row data with tabs for better readability
  }

  QString text = "\t" + header.join("\t") + "\n" + data.join("\n");    // Join rows with newlines

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapWidget::updateGraph(const joda::table::Table &&data, joda::plot::ColormapName colorMap, bool isImageView)
{
  if(isImageView) {
    mHeatmap->setGapsBetweenBoxes(1);
  } else {
    mHeatmap->setGapsBetweenBoxes(4);
  }
  mHeatmap->setColorMap(colorMap);
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
void HeatmapWidget::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);
  std::lock_guard<std::mutex> lock(mPaintMutex);
  // auto sizeToDraw = std::min(width(), height());
  auto withTmp   = width();
  auto heightTmp = height();
  if(heightTmp <= 0 || withTmp <= 0) {
    return;
  }
  auto graph = mHeatmap->plot({withTmp, heightTmp});
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
void HeatmapWidget::mousePressEvent(QMouseEvent *event)
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
void HeatmapWidget::mouseDoubleClickEvent(QMouseEvent *event)
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
