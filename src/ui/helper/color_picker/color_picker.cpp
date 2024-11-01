///
/// \file      color_picker.cpp
/// \author    Joachim Danmayr
/// \date      2024-10-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "color_picker.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qwidget.h>
#include <QMouseEvent>
#include <iostream>
#include <string>

namespace joda::ui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent)
{
  setMinimumHeight(200);
  setMinimumWidth(200);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

///
/// \brief      Painter
/// \author     Joachim Danmayr
///
void ColorPicker::calculateRangeBasedOnPoints()
{
  emit valueChanged();
}

///
/// \brief      Painter
/// \author     Joachim Danmayr
///
void ColorPicker::paintEvent(QPaintEvent *event)
{
  uint32_t width  = size().width();
  uint32_t height = size().height();
  int cx          = width / 2;
  int cy          = height / 2;

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);    // Enable smooth edges

  int radius = qMin(width, height) / 2 - 10;    // Radius for the circle, with a small padding

  // Loop through the angles (0-360 degrees for Hue)
  for(int y = -radius; y < radius; ++y) {
    for(int x = -radius; x < radius; ++x) {
      // Check if the point is within the circle
      if(x * x + y * y <= radius * radius) {
        double angle = qAtan2(y, x) * 180.0 / M_PI + 180.0;    // Get the angle in degrees (Hue)
        if(angle >= 360) {
          angle = 0;
        }
        double dist = qSqrt(x * x + y * y) / radius;    // Distance from the center (Saturation)
        // Convert HSV to QColor
        QColor color = QColor::fromHsv(angle, dist * 255, 255);
        // Set the pixel color
        painter.setPen(color);
        painter.drawPoint(cx + x, cy + y);

        // Update coordinates of points
        int32_t hue = angle;
        int32_t sat = (dist * 255);
        int32_t val = 0;

        for(auto &cpt : mClickPoints) {
          if(!cpt.isDraged && cpt.mSelectedHue == hue && std::abs(cpt.mSelectedSaturation - sat) <= 3) {
            cpt.mClickedPoint = {cx + x, cy + y};
          }
        }
      }
    }
  }

  // If the user has clicked, mark the selected point
  int startX = -1;
  int startY = -1;
  int actX   = -1;
  int actY   = -1;
  int idx    = 0;
  for(const auto &cpt : mClickPoints) {
    if(cpt.mClickedPoint != QPoint(-1, -1)) {
      if(cpt.isClicked) {
        painter.setPen(Qt::red);    // Red marker
      } else {
        painter.setPen(Qt::black);    // Black marker
      }
      // The grayscale value of the point is the vue
      painter.setBrush(QColor(cpt.mSelectedVal, cpt.mSelectedVal, cpt.mSelectedVal));    // White fill
      painter.drawEllipse(cpt.mClickedPoint, 5, 5);                                      // Draw a small circle around the clicked point
      auto txtPos = cpt.mClickedPoint;
      txtPos.setX(txtPos.x() + 10);
      if(idx != 1) {
        painter.drawText(txtPos, QString::number(cpt.mSelectedVal));
      }

      painter.setPen(Qt::black);    // Black marker
      if(actX < 0) {
        actX   = cpt.mClickedPoint.x();
        actY   = cpt.mClickedPoint.y();
        startX = cpt.mClickedPoint.x();
        startY = cpt.mClickedPoint.y();
      } else {
        painter.drawLine(actX, actY, cpt.mClickedPoint.x(), cpt.mClickedPoint.y());
        actX = cpt.mClickedPoint.x();
        actY = cpt.mClickedPoint.y();
      }
    }
    idx++;
  }
  if(startX >= 0) {
    painter.drawLine(actX, actY, startX, startY);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ColorPicker::wheelEvent(QWheelEvent *event)
{
  for(auto &circ : mClickPoints) {
    if(circ.isClicked) {
      if(event->angleDelta().y() > 0) {
        circ.mSelectedVal++;
        if(circ.mSelectedVal >= 255) {
          circ.mSelectedVal = 255;
        }
      } else {
        circ.mSelectedVal--;
        if(circ.mSelectedVal <= 0) {
          circ.mSelectedVal = 0;
        }
      }
      break;
    }
  }

  // Sync clickpoint 0 and 1
  mClickPoints[1].mSelectedVal = mClickPoints[0].mSelectedVal;

  update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ColorPicker::mousePressEvent(QMouseEvent *event)
{
  QPoint clickPos = event->pos();
  for(auto &circ : mClickPoints) {
    circ.isClicked = false;
  }

  // Check if the click is near the currently selected circle
  for(auto &circ : mClickPoints) {
    if(circ.mClickedPoint != QPoint(-1, -1) && (clickPos - circ.mClickedPoint).manhattanLength() < 10) {
      // Start dragging the circle if the click is close to the marker
      circ.isDraged  = true;
      circ.isClicked = true;
      return;
    }
  }
  update();
}

void ColorPicker::mouseMoveEvent(QMouseEvent *event)
{
  for(auto &circ : mClickPoints) {
    if(circ.isDraged) {
      // Update the position of the clicked point while dragging
      circ.mClickedPoint = event->pos();
      update();
    }
  }
}

void ColorPicker::mouseReleaseEvent(QMouseEvent *event)
{
  for(auto &circ : mClickPoints) {
    if(circ.isDraged) {
      // Stop dragging when the mouse button is released
      getValuesOfPoint(circ, event->pos());
      circ.isDraged = false;
    }
  }
}

void ColorPicker::getValuesOfPoint(ClickPoint &point, QPoint clickPos)
{
  int w  = width();
  int h  = height();
  int cx = w / 2;
  int cy = h / 2;

  int x = clickPos.x() - cx;
  int y = clickPos.y() - cy;

  int radius = qMin(w, h) / 2 - 10;

  // Check if the click is within the circle
  if(x * x + y * y <= radius * radius) {
    double angle = qAtan2(y, x) * 180.0 / M_PI + 180.0;    // Calculate angle for hue
    double dist  = qSqrt(x * x + y * y) / radius;          // Calculate distance for saturation

    point.mClickedPoint       = clickPos;
    point.mSelectedHue        = angle;
    point.mSelectedSaturation = dist * 255;

    // Trigger a repaint to mark the clicked point
    calculateRangeBasedOnPoints();
    update();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ColorPicker::ClickPoint::colorToCoordinates()
{
}

}    // namespace joda::ui
