///
/// \file      panel_image_view.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "panel_image_view.hpp"
#include <qtmetamacros.h>
#include <cstdint>
#include <string>
#include "backend/image_processing/image/image.hpp"
#include <opencv2/imgproc.hpp>

namespace joda::ui::qt {

////////////////////////////////////////////////////////////////
// Image view section
//
PanelImageView::PanelImageView(const joda::image::Image &imageReference, QWidget *parent) :
    QGraphicsView(parent), mActPixmapOriginal(imageReference)
{
  scene = new QGraphicsScene(this);
  setScene(scene);

  // Set up the view
  setRenderHint(QPainter::SmoothPixmapTransform);
  setDragMode(QGraphicsView::ScrollHandDrag);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  /*
  setMinimumWidth(width);
  setMinimumHeight(height);
  setMaximumWidth(width);
  setMaximumHeight(height);
  setFixedWidth(width);
  setFixedHeight(height);
  */

  setFrameShape(Shape::NoFrame);

  connect(this, &PanelImageView::updateImage, this, &PanelImageView::onUpdateImage);
}

void PanelImageView::setState(State state)
{
  mState = state;
  switch(mState) {
    case MOVE:
      viewport()->setCursor(QCursor(Qt::OpenHandCursor));
      break;
    case PAINT:
      viewport()->setCursor(QCursor(Qt::CrossCursor));
      break;
  }

  emit updateImage();
  emit onImageRepainted();
}

void PanelImageView::imageUpdated()
{
  if(mPlaceholderImageSet) {
    fitImageToScreenSize();
    mPlaceholderImageSet = false;
  }
  emit updateImage();
  emit onImageRepainted();
}

void PanelImageView::resetImage()
{
  mPlaceholderImageSet = true;
  fitImageToScreenSize();
  emit updateImage();
  emit onImageRepainted();
}

void PanelImageView::onUpdateImage()
{
  scene->setSceneRect(mActPixmapOriginal.getPixmap().rect());
  if(nullptr != mActPixmap) {
    scene->removeItem(mActPixmap);
    delete mActPixmap;
  }
  mActPixmap = scene->addPixmap(mActPixmapOriginal.getPixmap());

  scene->update();
  update();
  emit onImageRepainted();
}

void PanelImageView::mouseMoveEvent(QMouseEvent *event)
{
  if(isDragging) {
    // Calculate the difference in mouse position
    QPoint delta = event->pos() - lastPos;

    // Scroll the view
    verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());

    // Update the last position
    lastPos = event->pos();
  }
  emit onImageRepainted();
}

void PanelImageView::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // End dragging
    isDragging = false;
  }
}

void PanelImageView::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // Start dragging
    isDragging = true;
    lastPos    = event->pos();
  }
}

void PanelImageView::wheelEvent(QWheelEvent *event)
{
  zoomImage(event->angleDelta().y() > 0);
}

void PanelImageView::zoomImage(bool inOut)
{
  qreal zoomFactor = 1.05;
  if(inOut) {
    scale(zoomFactor, zoomFactor);
  } else {
    scale(1.0 / zoomFactor, 1.0 / zoomFactor);
  }
  emit onImageRepainted();

  /*
  QPointF center = mapToScene(viewport()->rect().center());
  centerOn(center);
  */
}

void PanelImageView::fitImageToScreenSize()
{
  resetTransform();
  float zoomFactor = static_cast<float>(width()) / static_cast<float>(mActPixmapOriginal.getPixmap().size().width());
  scale(zoomFactor, zoomFactor);
  emit onImageRepainted();
}

void PanelImageView::paintEvent(QPaintEvent *event)
{
  QGraphicsView::paintEvent(event);

  const float RECT_SIZE  = 80;
  const int RECT_START_X = 10;
  const int RECT_START_Y = 10;

  // Get the viewport rectangle
  QRect viewportRect = viewport()->rect();
  QRect rectangle(RECT_START_X, RECT_START_Y, RECT_SIZE, RECT_SIZE);    // Adjust the size as needed

  float zoomFactor     = viewportTransform().m11();
  float actImageWith   = RECT_SIZE * width() / (zoomFactor * scene->width());
  float actImageHeight = RECT_SIZE * height() / (zoomFactor * scene->width());

  float posX =
      (RECT_SIZE - actImageWith) * (float) horizontalScrollBar()->value() / (float) horizontalScrollBar()->maximum();
  float posY =
      (RECT_SIZE - actImageHeight) * (float) verticalScrollBar()->value() / (float) verticalScrollBar()->maximum();

  QRect viewPort(RECT_START_X + posX, RECT_START_Y + posY, actImageWith, actImageHeight);

  // Draw the rectangle
  QPainter painter(viewport());
  painter.setPen(QColor(173, 216, 230));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);            // Set the brush to no brush for transparent fill

  // Draw
  if(viewPort.width() < RECT_SIZE) {
    painter.drawRect(rectangle);
    painter.drawRect(viewPort);
  }

  // Draw histogram

  drawHistogram(mActPixmapOriginal.getImage());
}

void PanelImageView::drawHistogram(const cv::Mat &image)
{
  const float RECT_START_X  = 10;
  const float RECT_START_Y  = 12;
  const float RECT_HEIGHT   = 80;
  const float NR_OF_MARKERS = 8;
  float RECT_WIDTH          = static_cast<float>(width()) - (RECT_START_X * 2);

  int type  = image.type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth != CV_32F) {
    if(!image.empty()) {
      // Compute the histogram
      int histSize           = UINT16_MAX + 1;
      float range[]          = {0, UINT16_MAX + 1};
      const float *histRange = {range};
      bool uniform           = true;
      bool accumulate        = false;
      cv::Mat hist;
      cv::calcHist(&image, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);    //, uniform, accumulate);

      // Normalize the histogram to [0, histImage.height()]
      hist.at<float>(0) = 0;    // We don't want to display black
      cv::normalize(hist, hist, 0, RECT_HEIGHT, cv::NORM_MINMAX);

      // Draw the histogram

      QPainter painter(viewport());

      // Place for the histogram
      {
        painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        painter.setBrush(Qt::NoBrush);         // Set the brush to no brush for transparent fill
        QRect rectangle(width() - RECT_START_X - RECT_WIDTH, height() - RECT_START_Y - RECT_HEIGHT, RECT_WIDTH,
                        RECT_HEIGHT);    // Adjust the size as needed
        // painter.drawRect(rectangle);
      }

      float histOffset = mActPixmapOriginal.getHistogramOffset();
      float histZoom   = mActPixmapOriginal.getHitogramZoomFactor();

      int number = (float) UINT16_MAX / histZoom;

      float binWidth = (RECT_WIDTH / (float) number);
      QFont font;
      font.setPointSizeF(8);
      painter.setFont(font);

      int markerPos = number / NR_OF_MARKERS;

      for(int i = 1; i < number; i++) {
        float startX = ((float) width() - RECT_START_X - RECT_WIDTH) + (float) (i) *binWidth;
        float startY = (float) height() - RECT_START_Y;

        int idx = i + histOffset;
        if(idx > UINT16_MAX) {
          idx = UINT16_MAX;
        }
        uint16_t histValue = hist.at<float>(idx);
        painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        painter.drawLine(startX, startY, startX, startY - histValue);
        if(idx == mActPixmapOriginal.getUpperLevelContrast()) {
          painter.setPen(QColor(255, 0, 0));    // Set the pen color to light blue
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
          painter.drawLine(startX, startY, startX, startY - RECT_HEIGHT);
        }
        painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        if(i == 1 || i % markerPos == 0) {
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
        }

        /* painter.drawLine(binWidth * (i - 1), RECT_HEIGHT - qRound(hist.at<float>(i - 1)), binWidth * i,
                          RECT_HEIGHT - qRound(hist.at<float>(i)));*/
      }
    }
  }
}

void PanelImageView::enterEvent(QEnterEvent *)
{
}

void PanelImageView::leaveEvent(QEvent *)
{
}

}    // namespace joda::ui::qt
