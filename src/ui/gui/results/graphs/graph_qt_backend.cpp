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
#include <qcustomplot.h>
#include <qevent.h>
#include <QFile>
#include <QPainter>
#include <filesystem>
#include <thread>
#include "backend/helper/logger/console_logger.hpp"

namespace joda::ui::gui {

QtBackend::QtBackend(QWidget *parent) : QCustomPlot(parent), mColorMap(new QCPColorMap(xAxis, yAxis))
{
  setMouseTracking(true);    // <--- This line is crucial
}

QtBackend::~QtBackend()
{
}

void QtBackend::setNrOfRowsAndCols(int32_t rows, int32_t cols, const std::vector<std::vector<double>> &data)
{
  std::cout << "REplot" << std::endl;
  mRows        = rows;
  mCols        = cols;
  const int nx = cols;
  const int ny = rows;

  // Data for heatmap

  // Create color map
  mColorMap->data()->clear();
  mColorMap->data()->setSize(nx, ny);    // nx x ny cells
  mColorMap->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));
  mColorMap->setGradient(QCPColorGradient::gpJet);
  mColorMap->setInterpolate(false);    // disables smooth interpolation — colors jump at cell edges

  // Fill color map data
  for(int x = 0; x < nx; ++x) {
    for(int y = 0; y < ny; ++y) {
      mColorMap->data()->setCell(x, y, data[x][y]);
    }
  }

  // Set axis ranges to fit the map
  xAxis->setRange(0, nx);
  yAxis->setRange(0, ny);

  // Invert y axis so origin is bottom-left
  yAxis->setRangeReversed(true);
  clearItems();
  // Add labels in each cell
  for(int x = 0; x < nx; ++x) {
    for(int y = 0; y < ny; ++y) {
      QCPItemText *textLabel = new QCPItemText(this);
      textLabel->setPositionAlignment(Qt::AlignCenter);
      textLabel->position->setCoords(x + 0.5, y + 0.5);    // center of cell
      textLabel->setText(QString::number(data[x][y]));
      textLabel->setFont(QFont(font().family(), 12));
      textLabel->setColor(Qt::black);
    }
  }

  // =========================================
  // PLot grid
  // ==========================================
  {
    /*
    double xMin = mColorMap->data()->keyRange().lower;
    double xMax = mColorMap->data()->keyRange().upper;
    double yMin = mColorMap->data()->valueRange().lower;
    double yMax = mColorMap->data()->valueRange().upper;

    int nx = mColorMap->data()->keySize();
    int ny = mColorMap->data()->valueSize();

    double dx = (xMax - xMin) / nx;
    double dy = (yMax - yMin) / ny;

    // Draw vertical grid lines
    for(int i = 0; i <= nx; ++i) {
      double x   = xMin + i * dx;
      auto *line = new QCPItemLine(this);
      line->setPositions(QVector2D(x, yMin), QVector2D(x, yMax));
    }

    // Draw horizontal grid lines
    for(int j = 0; j <= ny; ++j) {
      double y = yMin + j * dy;
      addItem(new QCPItemLine(this))->setPositions(QVector2D(xMin, y), QVector2D(xMax, y));
    }
      */
  }

  mColorMap->rescaleDataRange();
  rescaleAxes();
  replot();
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
  QCustomPlot::paintEvent(event);
  std::lock_guard<std::mutex> lock(mPaintMutex);
  if((svgRenderer != nullptr) && svgRenderer->isValid()) {
    QPainter painter(this);

    QSizeF svgSize    = svgRenderer->defaultSize();    // original SVG size
    QSizeF widgetSize = size();

    // Calculate scaled size preserving aspect ratio
    QSizeF scaledSize = svgSize;
    scaledSize.scale(widgetSize, Qt::KeepAspectRatio);

    // Center the SVG in the widget
    targetRect = QRectF((widgetSize.width() - scaledSize.width()) / 2.0, (widgetSize.height() - scaledSize.height()) / 2.0, scaledSize.width(),
                        scaledSize.height());

    svgRenderer->render(&painter, targetRect);

    //
    // This is a grid used for mouse events
    //
    QPen redPen(Qt::red);
    redPen.setWidth(1);                // Optional: set line width
    redPen.setStyle(Qt::SolidLine);    // <-- Make it dashed
    painter.setPen(redPen);
    float offsetX = (targetRect.width() / 7.7);
    float offsetY = (targetRect.height() / 13.3);
    float width   = ((targetRect.width() - 2.05 * offsetX) / mCols);
    float height  = ((targetRect.height() - 2.5 * offsetY) / mRows);
    mRects.clear();
    int idx = 0;
    for(float col = 0; col < mCols; col++) {
      for(float row = 0; row < mRows; row++) {
        float startY = offsetY + targetRect.y() + row * height;
        float startX = offsetX + targetRect.x() + col * width;
        auto rect    = QRectF(startX, startY, width, height);
        mRects.push_back({rect, QPoint{static_cast<int>(col), static_cast<int>(row)}});
        if(mSelectedIndex == idx) {
          QRect tmp(rect.x() + 2, rect.y() + 2, rect.width() - 4, rect.height() - 4);
          painter.drawRect(tmp);
        }
        idx++;
      }
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
void QtBackend::mousePressEvent(QMouseEvent *event)
{
  QWidget::mouseMoveEvent(event);
  QPoint pos = event->pos();
  int idx    = 0;
  for(const auto &[rect, pt] : mRects) {
    if(rect.contains(pos)) {
      emit onGraphClicked(pt.y(), pt.x());
      mSelectedIndex = idx;
      update();
      return;
    }
    idx++;
  }
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
  QPoint pos = event->pos();
  int idx    = 0;
  for(const auto &[rect, pt] : mRects) {
    if(rect.contains(pos)) {
      emit onGraphDoubleClicked(pt.y(), pt.x());
      mSelectedIndex = idx;
      update();
      return;
    }
    idx++;
  }
}

}    // namespace joda::ui::gui
