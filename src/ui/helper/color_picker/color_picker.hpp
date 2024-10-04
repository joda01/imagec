///
/// \file      color_picker.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qtmetamacros.h>
#include <qwidget.h>
#include "backend/enums/types.hpp"

namespace joda::ui {

///
/// \class
/// \author
/// \brief
///
class ColorPicker : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  ColorPicker(QWidget *parent = nullptr);

  void setValue(const std::tuple<joda::enums::HsvColor, joda::enums::HsvColor, joda::enums::HsvColor> &triangle)
  {
    mClickPoints[0].mSelectedHue        = std::get<0>(triangle).hue;
    mClickPoints[0].mSelectedSaturation = std::get<0>(triangle).sat;

    mClickPoints[1].mSelectedHue        = std::get<1>(triangle).hue;
    mClickPoints[1].mSelectedSaturation = std::get<1>(triangle).sat;

    mClickPoints[2].mSelectedHue        = std::get<2>(triangle).hue;
    mClickPoints[2].mSelectedSaturation = std::get<2>(triangle).sat;

    update();
    emit valueChanged();
  }

  auto getValue() -> std::tuple<joda::enums::HsvColor, joda::enums::HsvColor, joda::enums::HsvColor>
  {
    return {{.hue = mClickPoints[0].mSelectedHue, .sat = mClickPoints[0].mSelectedSaturation, .val = 0},
            {.hue = mClickPoints[1].mSelectedHue, .sat = mClickPoints[1].mSelectedSaturation, .val = 0},
            {.hue = mClickPoints[2].mSelectedHue, .sat = mClickPoints[2].mSelectedSaturation, .val = 255}};
  }

signals:
  void valueChanged();

private:
  /////////////////////////////////////////////////////
  struct ClickPoint
  {
    QPoint mClickedPoint{0, 0};         // To store the location of the clicked point
    int32_t mSelectedHue        = 0;    // Store the selected hue
    int32_t mSelectedSaturation = 0;    // Store the selected saturation
    bool isDraged               = false;
    void colorToCoordinates();
  };

  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void calculateRangeBasedOnPoints();
  void getValuesOfPoint(ClickPoint &point, QPoint clickPos);

  std::array<ClickPoint, 3> mClickPoints;
};

}    // namespace joda::ui
