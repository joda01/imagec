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
  PreviewLabel(QWidget *parent = nullptr);
  void setPixmap(const QPixmap &pix, int width, int height);

protected:
  void mouseMoveEvent(QMouseEvent *event) override;

  void enterEvent(QEnterEvent *) override;

  void leaveEvent(QEvent *) override;

  void wheelEvent(QWheelEvent *event) override;

  void paintEvent(QPaintEvent *event) override;

private:
  void magnify(const QPoint &mousePos);
  void resetMagnifier();

  QRect magnifierRectFromMousePos(const QPoint &mousePos);

  QPixmap originalPixmap;
  QSize magnifierSize{200, 200};
  qreal magnificationFactor;
  QPoint magnifierPosition;
  bool magnifierActive{false};
};

}    // namespace joda::ui::qt
