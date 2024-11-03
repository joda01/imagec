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
/// \brief
/// \author     Joachim Danmayr
///
void ColorPicker::setValue(const std::tuple<joda::enums::HsvColor, joda::enums::HsvColor> &triangle)
{
  for(auto &cpt : mClickPoints) {
    cpt.isDraged  = false;
    cpt.isClicked = false;
  }

  auto calcHueVorMiddle = [&]() {
    double from = std::get<0>(triangle).hue;
    double to   = std::get<1>(triangle).hue;
    if(from > to) {
      from = 360 - from;
    }
    return static_cast<int32_t>(to - ((to - from) / 2.0));
  };

  mClickPoints[0].mSelectedHue        = std::get<0>(triangle).hue;
  mClickPoints[0].mSelectedSaturation = std::get<1>(triangle).sat;
  mClickPoints[0].mSelectedVal        = std::get<0>(triangle).val;

  mClickPoints[1].mSelectedHue        = std::get<1>(triangle).hue;
  mClickPoints[1].mSelectedSaturation = std::get<1>(triangle).sat;
  mClickPoints[1].mSelectedVal        = std::get<0>(triangle).val;

  mClickPoints[2].mSelectedHue        = calcHueVorMiddle();
  mClickPoints[2].mSelectedSaturation = std::get<0>(triangle).sat;
  mClickPoints[2].mSelectedVal        = std::get<1>(triangle).val;

  mClickPoints[1].mSelectedVal = mClickPoints[0].mSelectedVal;

  adjustPoints(-1);
  update();
  emit valueChanged();
}

///
/// \brief
/// \author     Joachim Danmayr
///
auto ColorPicker::getValue() -> std::tuple<joda::enums::HsvColor, joda::enums::HsvColor>
{
  return {{.hue = mClickPoints[0].mSelectedHue, .sat = mClickPoints[2].mSelectedSaturation, .val = mClickPoints[0].mSelectedVal},
          {.hue = mClickPoints[1].mSelectedHue, .sat = mClickPoints[0].mSelectedSaturation, .val = mClickPoints[2].mSelectedVal}};
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

        // Update coordinates of points
        int32_t hue   = angle;
        int32_t sat   = (dist * 255);
        int32_t alpha = 64;

        if(mClickPoints[0].mSelectedHue > mClickPoints[1].mSelectedHue) {
          if((hue >= mClickPoints[0].mSelectedHue || hue <= mClickPoints[1].mSelectedHue) && sat >= mClickPoints[2].mSelectedSaturation &&
             sat <= mClickPoints[1].mSelectedSaturation) {
            alpha = 255;
          }
        } else {
          if(hue >= mClickPoints[0].mSelectedHue && hue <= mClickPoints[1].mSelectedHue && sat >= mClickPoints[2].mSelectedSaturation &&
             sat <= mClickPoints[1].mSelectedSaturation) {
            alpha = 255;
          }
        }

        // Convert HSV to QColor
        QColor color = QColor::fromHsv(angle, dist * 255, 255, alpha);
        // Set the pixel color
        painter.setPen(color);
        painter.drawPoint(cx + x, cy + y);

        for(std::size_t i = 0; i < mClickPoints.size(); ++i) {
          auto &cpt = mClickPoints[i];
          if(!cpt.isDraged && std::abs(cpt.mSelectedHue - hue) <= 5 && std::abs(cpt.mSelectedSaturation - sat) <= 5) {
            cpt.mClickedPoint = {cx + x, cy + y};
          }
        }
      }
    }
  }

  // If the user has clicked, mark the selected point
  int idx = 0;
  for(std::size_t i = 0; i < mClickPoints.size(); ++i) {
    auto &cpt = mClickPoints[i];

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
      QString name;
      if(i == 0) {
        name = " (from)";
      } else if(i == 1) {
        name = " (to)";
      }
      painter.drawText(txtPos, QString::number(cpt.mSelectedVal) + name);

      painter.setPen(Qt::black);    // Black marker
    }
    idx++;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ColorPicker::adjustPoints(int32_t modifiedPtIdx)
{
  auto calcHueVorMiddle = [this]() {
    double from = mClickPoints[0].mSelectedHue;
    double to   = mClickPoints[1].mSelectedHue;
    if(from > to) {
      from = 360 - from;
    }
    return static_cast<int32_t>(to - ((to - from) / 2.0));
  };

  if(modifiedPtIdx == 0) {
    mClickPoints[1].mSelectedSaturation = mClickPoints[0].mSelectedSaturation;
    mClickPoints[1].mSelectedVal        = mClickPoints[0].mSelectedVal;
    mClickPoints[2].mSelectedHue        = calcHueVorMiddle();
  } else if(modifiedPtIdx == 1) {
    mClickPoints[0].mSelectedSaturation = mClickPoints[1].mSelectedSaturation;
    mClickPoints[0].mSelectedVal        = mClickPoints[1].mSelectedVal;
    mClickPoints[2].mSelectedHue        = calcHueVorMiddle();
  } else if(modifiedPtIdx == 2) {
    mClickPoints[2].mSelectedHue = calcHueVorMiddle();
  }

  if(mClickPoints[1].mSelectedSaturation < mClickPoints[2].mSelectedSaturation) {
    auto tmp                            = mClickPoints[2].mSelectedSaturation;
    mClickPoints[2].mSelectedSaturation = mClickPoints[1].mSelectedSaturation;
    mClickPoints[0].mSelectedSaturation = tmp;
    mClickPoints[1].mSelectedSaturation = tmp;
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
  for(std::size_t i = 0; i < mClickPoints.size(); ++i) {
    auto &circ = mClickPoints[i];
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
      adjustPoints(i);
      emit valueChanged();
      break;
    }
  }

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
      circ.mOldClickPos = circ.mClickedPoint;
      circ.isDraged     = true;
      circ.isClicked    = true;
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
  for(std::size_t i = 0; i < mClickPoints.size(); ++i) {
    auto &circ = mClickPoints[i];
    if(circ.isDraged) {
      // Stop dragging when the mouse button is released
      getValuesOfPoint(i, circ, event->pos());
      circ.isDraged = false;
    }
  }
}

void ColorPicker::getValuesOfPoint(int32_t idx, ClickPoint &point, QPoint clickPos)
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

  } else {
    point.mClickedPoint = point.mOldClickPos;
  }
  // Trigger a repaint to mark the clicked point
  adjustPoints(idx);
  emit valueChanged();
  update();
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
