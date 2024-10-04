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
#include <qwidget.h>
#include "ui/container/dialog_command_selection/dialog_command_selection.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

AddCommandButtonBase::AddCommandButtonBase(joda::settings::Pipeline &settings, PanelPipelineSettings *pipelineStepSettingsUi,
                                           const settings::PipelineStep *pipelineStepBefore, InOuts outOfStepBefore, WindowMain *parent) :
    mParent(parent),
    mPipelineStepBefore(pipelineStepBefore), mSettings(settings), pipelineStepSettingsUi(pipelineStepSettingsUi), mOutOfStepBefore(outOfStepBefore)
{
  setObjectName("addCommandButton");
  setContentsMargins(0, 0, 0, 0);
  setFixedHeight(10);
  mSelectionDialog = new DialogCommandSelection(mSettings, pipelineStepSettingsUi, mPipelineStepBefore, mOutOfStepBefore, mParent);
}

void AddCommandButtonBase::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  if(!mMouseEntered) {
    painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
  } else {
    painter.setPen(QPen(Qt::black, 1, Qt::DotLine));
  }

  // Calculate the center point of the line
  int centerX = width() / 2;
  int centerY = height() / 2;

  // Draw the line
  painter.drawLine(0, centerY, width(), centerY);

  // Draw the plus symbol
  painter.drawText(centerX - 5, centerY - 5, 10, 10, Qt::AlignCenter, "+");
}

void AddCommandButtonBase::mousePressEvent(QMouseEvent *event)
{
  mSelectionDialog->exec();
}

void AddCommandButtonBase::enterEvent(QEnterEvent *event)
{
  QWidget::enterEvent(event);
  mMouseEntered = true;
  setCursor(Qt::PointingHandCursor);
  repaint();
}
void AddCommandButtonBase::leaveEvent(QEvent *event)
{
  QWidget::leaveEvent(event);
  mMouseEntered = false;
  setCursor(Qt::ArrowCursor);

  repaint();
}

void AddCommandButtonBase::onAddCommandClicked()
{
}

void AddCommandButtonBase::setInOutBefore(InOuts inout)
{
  mOutOfStepBefore = inout;
  mSelectionDialog->setInOutBefore(inout);
}

}    // namespace joda::ui
