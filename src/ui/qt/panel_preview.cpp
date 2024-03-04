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
#include <qlabel.h>
#include <QtWidgets>
#include <iostream>
#include <string>

namespace joda::ui::qt {

PreviewLabel::PreviewLabel(QWidget *parent) : QLabel(parent), magnificationFactor(1.0)
{
  setMouseTracking(true);
}

void PreviewLabel::setPixmap(const QPixmap &pix, int width, int height)
{
  QLabel::setPixmap(pix.scaled(width, height));
  originalPixmap = pix;
}

void PreviewLabel::mouseMoveEvent(QMouseEvent *event)
{
  magnify(event->pos());
}

void PreviewLabel::enterEvent(QEnterEvent *)
{
  magnificationFactor = 1.0;
  setCursor(Qt::CrossCursor);
}

void PreviewLabel::leaveEvent(QEvent *)
{
  resetMagnifier();
}

void PreviewLabel::wheelEvent(QWheelEvent *event)
{
  int delta = event->angleDelta().y();
  if(delta > 0) {
    magnificationFactor++;
  } else {
    if(magnificationFactor > 1) {
      magnificationFactor--;
    }
  }
  update();
}

void PreviewLabel::paintEvent(QPaintEvent *event)
{
  QLabel::paintEvent(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);

  if(magnifierActive) {
    QRect magnifierRect     = magnifierRectFromMousePos(magnifierPosition);
    QPixmap magnifiedPixmap = originalPixmap.copy(magnifierRect);
    QPixmap zoomedPixmap    = magnifiedPixmap.scaled((int) ((float) magnifiedPixmap.width() * magnificationFactor),
                                                     (int) ((float) magnifiedPixmap.height() * magnificationFactor),
                                                     Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QRect cropRect((zoomedPixmap.width() - magnifiedPixmap.width()) / 2.0f,
                   (zoomedPixmap.height() - magnifiedPixmap.height()) / 2.0f, magnifierRect.width(),
                   magnifierRect.height());

    zoomedPixmap = zoomedPixmap.copy(cropRect);
    painter.fillRect(magnifierRect, Qt::white);
    painter.drawPixmap(magnifierRect, zoomedPixmap);
  }
}

void PreviewLabel::magnify(const QPoint &mousePos)
{
  magnifierPosition = mousePos;
  magnifierActive   = true;
  update();
}

void PreviewLabel::resetMagnifier()
{
  magnifierActive = false;
  update();
}

QRect PreviewLabel::magnifierRectFromMousePos(const QPoint &mousePos)
{
  int halfWidth  = magnifierSize.width() / 2;
  int halfHeight = magnifierSize.height() / 2;
  int x          = mousePos.x() - halfWidth;
  int y          = mousePos.y() - halfHeight;
  if(x < 0) {
    x = 0;
  }
  if(y < 0) {
    y = 0;
  }

  return QRect(x, y, magnifierSize.width(), magnifierSize.height());
}

}    // namespace joda::ui::qt
