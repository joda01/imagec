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
#include <qcombobox.h>
#include "color_item.hpp"

ColorComboBox::ColorComboBox(QWidget *parent) : QComboBox(parent)
{
  auto *delegate = new ColorRectDelegate(this);
  setItemDelegate(delegate);
}

ColorComboBox::~ColorComboBox()
{
}

void ColorComboBox::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  // Use the default style options for the combo box
  QStyleOptionComboBox opt;
  initStyleOption(&opt);

  // Draw the default combo box, including the frame and drop-down arrow
  style()->drawComplexControl(QStyle::CC_ComboBox, &opt, &painter, this);

  // Get the current index
  int index = currentIndex();
  if(index >= 0) {
    // Retrieve the background color and text from the model
    QVariant bgColor = model()->data(model()->index(index, 0), Qt::BackgroundRole);
    QVariant text    = model()->data(model()->index(index, 0), Qt::DisplayRole);

    // Set the background color
    QRect colorRect = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
    colorRect.setY(colorRect.y() + colorRect.height() / 2 - 4);
    colorRect.setX(colorRect.x() + 4);
    colorRect.setWidth(8);
    colorRect.setHeight(8);

    painter.fillRect(colorRect, bgColor.value<QColor>());

    // Draw the text
    QRect textRect = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
    textRect.setX(textRect.x() + 12);
    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(textRect.adjusted(5, 0, -5, 0), Qt::AlignVCenter | Qt::AlignLeft, text.toString());
  }
}
