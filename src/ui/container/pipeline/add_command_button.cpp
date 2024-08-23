///
/// \file      add_command_button.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "add_command_button.hpp"
#include <qnamespace.h>

namespace joda::ui {

AddCommandButtonBase::AddCommandButtonBase()
{
  setObjectName("addCommandButton");
  // const QIcon myIcon(":/icons/outlined/icons8-plus-math-50.png");
  // setIcon(myIcon.pixmap(12, 12));
  setContentsMargins(0, 0, 0, 0);
  setFixedHeight(10);
  // connect(this, &QPushButton::clicked, this, &AddCommandButtonBase::onAddCommandClicked);
}

void AddCommandButtonBase::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));

  // Calculate the center point of the line
  int centerX = width() / 2;
  int centerY = height() / 2;

  // Draw the line
  painter.drawLine(0, centerY, width(), centerY);

  // Draw the plus symbol
  painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
  painter.drawText(centerX - 5, centerY - 5, 10, 10, Qt::AlignCenter, "+");
}

void AddCommandButtonBase::onAddCommandClicked()
{
}
}    // namespace joda::ui
