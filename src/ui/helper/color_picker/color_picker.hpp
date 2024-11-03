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
#include <iostream>
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

  void setValue(const std::tuple<joda::enums::HsvColor, joda::enums::HsvColor> &triangle);
  auto getValue() -> std::tuple<joda::enums::HsvColor, joda::enums::HsvColor>;

signals:
  void valueChanged();

private:
  /////////////////////////////////////////////////////
  struct ClickPoint
  {
    QPoint mOldClickPos{0, 0};          // To store the location of the clicked point
    QPoint mClickedPoint{0, 0};         // To store the location of the clicked point
    int32_t mSelectedHue        = 0;    // Store the selected hue
    int32_t mSelectedSaturation = 0;    // Store the selected saturation
    int32_t mSelectedVal        = 0;    // Store the selected vue
    bool isDraged               = false;
    bool isClicked              = false;
    void colorToCoordinates();
  };

  /////////////////////////////////////////////////////
  void adjustPoints(int32_t modifiedPtIdx);
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void getValuesOfPoint(int32_t idx, ClickPoint &point, QPoint clickPos);

  std::array<ClickPoint, 3> mClickPoints;
};

}    // namespace joda::ui
