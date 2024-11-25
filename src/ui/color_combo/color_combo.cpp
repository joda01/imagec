///
/// \file      color_combo.cpp
/// \author    Joachim Danmayr
/// \date      2024-11-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "color_combo.hpp"

ColorComboBox::~ColorComboBox()
{
}

void ColorComboBox::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  // Draw the combo box as usual
  QStyleOptionComboBox opt;
  initStyleOption(&opt);
  style()->drawComplexControl(QStyle::CC_ComboBox, &opt, &painter, this);

  // Get the current index
  int index = currentIndex();
  if(index >= 0) {
    // Get the model's background color for the current item
    QVariant bgColor = model()->data(model()->index(index, 0), Qt::BackgroundRole);
    QVariant text    = model()->data(model()->index(index, 0), Qt::DisplayRole);

    // Set the background color and draw it
    painter.fillRect(opt.rect, bgColor.value<QColor>());

    // Draw the text
    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(opt.rect.adjusted(5, 0, -5, 0), Qt::AlignVCenter | Qt::AlignLeft, text.toString());
  }
}
