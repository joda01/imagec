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

namespace joda::ui::qt {

////////////////////////////////////////////////////////////////
// Image view section
//
PanelImageView::PanelImageView(QWidget *parent) : QGraphicsView(parent)
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

void PanelImageView::setPixmap(const QPixmap &pixIn)
{
  mActPixmapOriginal = pixIn;

  if(mPlaceholderImageSet) {
    fitImageToScreenSize();
    mPlaceholderImageSet = false;
  }
  emit updateImage();
}

void PanelImageView::resetImage()
{
  mActPixmapOriginal   = QPixmap(PLACEHOLDER);
  mPlaceholderImageSet = true;
  fitImageToScreenSize();
  emit updateImage();
}

void PanelImageView::onUpdateImage()
{
  scene->setSceneRect(mActPixmapOriginal.rect());

  if(nullptr == mActPixmap) {
    mActPixmap = scene->addPixmap(mActPixmapOriginal);
  } else {
    scene->removeItem(mActPixmap);
    mActPixmap = scene->addPixmap(mActPixmapOriginal);
  }

  scene->update();
  update();
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
}

void PanelImageView::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // End dragging
    isDragging = false;
  }
}

void PanelImageView::enterEvent(QEnterEvent *)
{
}

void PanelImageView::leaveEvent(QEvent *)
{
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
  qreal zoomFactor = 1.05;

  if(event->angleDelta().y() > 0) {
    // Zoom in
    scale(zoomFactor, zoomFactor);
  } else if(event->angleDelta().y() < 0) {
    // Zoom out
    scale(1.0 / zoomFactor, 1.0 / zoomFactor);
  }
}

void PanelImageView::zoomImage(bool inOut)
{
  qreal zoomFactor = 1.05;
  if(inOut) {
    scale(zoomFactor, zoomFactor);
  } else {
    scale(1.0 / zoomFactor, 1.0 / zoomFactor);
  }

  QPointF center = mapToScene(viewport()->rect().center());
  // Set the new center point after scaling
  centerOn(center);
  // Ensure the view doesn't go beyond the scene boundaries
  // ensureVisible(sceneRect());
}

void PanelImageView::fitImageToScreenSize()
{
  resetTransform();
  float zoomFactor = static_cast<float>(width()) / static_cast<float>(mActPixmapOriginal.size().width());
  scale(zoomFactor, zoomFactor);
}
}    // namespace joda::ui::qt
