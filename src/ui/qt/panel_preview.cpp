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
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <QtWidgets>
#include <iostream>
#include <string>

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelPreview::PanelPreview(int width, int height, QWidget *parent) : mPreviewLabel(width, height, parent)
{
  QVBoxLayout *hLayout = new QVBoxLayout();
  hLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(hLayout);
  hLayout->addWidget(&mPreviewLabel);
  hLayout->addWidget(createToolBar());

  mPreviewInfo = new QLabel("-");
  hLayout->addWidget(mPreviewInfo);

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
  layout->setContentsMargins(0, 0, 0, 0);

  container->setLayout(layout);

  QButtonGroup *buttonGroup = new QButtonGroup();

  QPushButton *move = new QPushButton(QIcon(":/icons/outlined/icons8-hand-50.png"), "");
  move->setCheckable(true);
  move->setToolTip("Move the image");
  move->setChecked(true);
  buttonGroup->addButton(move);
  layout->addWidget(move);

  QPushButton *fitToScreen = new QPushButton(QIcon(":/icons/outlined/icons8-full-image-50.png"), "");
  fitToScreen->setToolTip("Fit image to screen");
  connect(fitToScreen, &QPushButton::pressed, this, &PanelPreview::onFitImageToScreenSizeClicked);
  layout->addWidget(fitToScreen);

  QPushButton *zoomIn = new QPushButton(QIcon(":/icons/outlined/icons8-zoom-in-50.png"), "");
  zoomIn->setToolTip("Zoom in");
  connect(zoomIn, &QPushButton::pressed, this, &PanelPreview::onZoomInClicked);
  layout->addWidget(zoomIn);

  QPushButton *zoomOut = new QPushButton(QIcon(":/icons/outlined/icons8-zoom-out-50.png"), "");
  zoomOut->setToolTip("Zoom out");
  connect(zoomOut, &QPushButton::pressed, this, &PanelPreview::onZoomOutClicked);
  layout->addWidget(zoomOut);

  layout->addStretch();

  return container;
}

void PanelPreview::onZoomInClicked()
{
  mPreviewLabel.zoomImage(true);
}
void PanelPreview::onZoomOutClicked()
{
  mPreviewLabel.zoomImage(false);
}

void PanelPreview::onFitImageToScreenSizeClicked()
{
  mPreviewLabel.fitImageToScreenSize();
}

////////////////////////////////////////////////////////////////
// Image view section
//
PreviewLabel::PreviewLabel(int width, int height, QWidget *parent) : QGraphicsView(parent)
{
  scene = new QGraphicsScene(this);
  setScene(scene);

  // Set up the view
  setRenderHint(QPainter::SmoothPixmapTransform);
  setDragMode(QGraphicsView::ScrollHandDrag);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMinimumWidth(width);
  setMinimumHeight(height);
  setMaximumWidth(width);
  setMaximumHeight(height);
  setFixedWidth(width);
  setFixedHeight(height);

  setFrameShape(Shape::NoFrame);

  connect(this, &PreviewLabel::updateImage, this, &PreviewLabel::onUpdateImage);
}

void PreviewLabel::setPixmap(const QPixmap &pixIn)
{
  mActPixmapOriginal = pixIn;

  if(mPlaceholderImageSet) {
    fitImageToScreenSize();
    mPlaceholderImageSet = false;
  }
  emit updateImage();
}

void PreviewLabel::resetImage()
{
  mActPixmapOriginal   = QPixmap(PLACEHOLDER);
  mPlaceholderImageSet = true;
  fitImageToScreenSize();
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

void PreviewLabel::zoomImage(bool inOut)
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

void PreviewLabel::fitImageToScreenSize()
{
  resetTransform();
  float zoomFactor = static_cast<float>(width()) / static_cast<float>(mActPixmapOriginal.size().width());
  scale(zoomFactor, zoomFactor);
}

}    // namespace joda::ui::qt
