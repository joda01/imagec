///
/// \file      panel_preview.hpp
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

#include "panel_preview.hpp"
#include <qboxlayout.h>
#include <qgraphicsview.h>
#include <qicon.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <QtWidgets>
#include <iostream>
#include <string>

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelPreview::PanelPreview(QWidget *parent) : mPreviewLabel(parent)
{
  QVBoxLayout *hLayout = new QVBoxLayout();
  this->setLayout(hLayout);
  hLayout->addWidget(&mPreviewLabel);
  hLayout->addWidget(createToolBar());
  hLayout->addStretch();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
QWidget *PanelPreview::createToolBar()
{
  QWidget *container  = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  container->setLayout(layout);

  // QPushButton *zoon = new QPushButton(QIcon(":/icons/outlined/icons8-search-50.png"), "");
  // zoon->setCheckable(true);
  // layout->addWidget(zoon);
  layout->addStretch();

  return container;
}

////////////////////////////////////////////////////////////////
// Image view section
//
PreviewLabel::PreviewLabel(QWidget *parent) : QGraphicsView(parent)
{
  scene = new QGraphicsScene(this);
  setScene(scene);

  // Set up the view
  setRenderHint(QPainter::SmoothPixmapTransform);
  setDragMode(QGraphicsView::ScrollHandDrag);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  connect(this, &PreviewLabel::updateImage, this, &PreviewLabel::onUpdateImage);
}

void PreviewLabel::setPixmap(const QPixmap &pixIn, int width, int height)
{
  setMinimumWidth(width);
  setMinimumHeight(height);
  setMaximumWidth(width);
  setMaximumHeight(height);
  setFixedWidth(width);
  setFixedHeight(height);

  mActPixmapOriginal = pixIn;

  emit updateImage();
}

void PreviewLabel::onUpdateImage()
{
  scene->setSceneRect(mActPixmapOriginal.rect());

  if(nullptr == mActPixmap) {
    mActPixmap = scene->addPixmap(mActPixmapOriginal);
  } else {
    scene->removeItem(mActPixmap);
    mActPixmap = scene->addPixmap(mActPixmapOriginal);
  }
  fitImageToScreenSize();

  scene->update();
  update();
}

void PreviewLabel::mouseMoveEvent(QMouseEvent *event)
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

void PreviewLabel::paintEvent(QPaintEvent *event)
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
  painter.drawRect(rectangle);
  painter.drawRect(viewPort);
}

void PreviewLabel::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // End dragging
    isDragging = false;
  }
}

void PreviewLabel::enterEvent(QEnterEvent *)
{
}

void PreviewLabel::leaveEvent(QEvent *)
{
}

void PreviewLabel::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // Start dragging
    isDragging = true;
    lastPos    = event->pos();
  }
}

void PreviewLabel::wheelEvent(QWheelEvent *event)
{
  qreal zoomFactor = 1.05;
  if(event->pixelDelta().ry() > 0) {
    scale(zoomFactor, zoomFactor);
  } else {
    scale(1.0 / zoomFactor, 1.0 / zoomFactor);
  }
}

void PreviewLabel::fitImageToScreenSize()
{
  float zoomFactor = static_cast<float>(width()) / static_cast<float>(scene->width());
  scale(zoomFactor, zoomFactor);
}

}    // namespace joda::ui::qt
