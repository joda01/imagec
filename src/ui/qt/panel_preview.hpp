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

#pragma once

#include <qlabel.h>
#include <QtWidgets>
#include <iostream>
#include <string>

namespace joda::ui::qt {

class PreviewLabel : public QLabel
{
public:
  PreviewLabel(QWidget *parent = nullptr) : QLabel(parent), magnificationFactor(4.0)
  {
    setMouseTracking(true);
  }

  void setPixmap(const QPixmap &pix, int width, int height)
  {
    QLabel::setPixmap(pix.scaled(width, height));
    originalPixmap = pix;
  }

protected:
  void mouseMoveEvent(QMouseEvent *event) override
  {
    magnify(event->pos());
  }

  void enterEvent(QEnterEvent *) override
  {
    magnificationFactor = 4.0;
    setCursor(Qt::CrossCursor);
  }

  void leaveEvent(QEvent *) override
  {
    resetMagnifier();
  }

  void wheelEvent(QWheelEvent *event) override
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

  void paintEvent(QPaintEvent *event) override
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
      std::cout << "P " << std::to_string(originalPixmap.width()) << std::endl;
    }
  }

private:
  void magnify(const QPoint &mousePos)
  {
    magnifierPosition = mousePos;
    magnifierActive   = true;
    update();
  }

  void resetMagnifier()
  {
    magnifierActive = false;
    update();
  }

  QRect magnifierRectFromMousePos(const QPoint &mousePos)
  {
    int halfWidth  = magnifierSize.width() / 2;
    int halfHeight = magnifierSize.height() / 2;
    int x          = qMax(0, qMin(mousePos.x() - halfWidth, originalPixmap.width() - magnifierSize.width()));
    int y          = qMax(0, qMin(mousePos.y() - halfHeight, originalPixmap.height() - magnifierSize.height()));

    return QRect(x, y, magnifierSize.width(), magnifierSize.height());
  }

  QPixmap originalPixmap;
  QSize magnifierSize{200, 200};
  qreal magnificationFactor;
  QPoint magnifierPosition;
  bool magnifierActive{false};
};

}    // namespace joda::ui::qt
