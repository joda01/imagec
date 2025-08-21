///
/// \file      add_command_button.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "add_command_button.hpp"
#include <qnamespace.h>
#include <qwidget.h>
#include "ui/gui/editor/widget_pipeline/dialog_command_selection/dialog_command_selection.hpp"
#include "ui/gui/editor/window_main.hpp"

namespace joda::ui::gui {

AddCommandButtonBase::AddCommandButtonBase(std::shared_ptr<DialogCommandSelection> &dialogCommandSelection, joda::settings::Pipeline &settings,
                                           PanelPipelineSettings *pipelineStepSettingsUiIn, const settings::PipelineStep *pipelineStepBefore,
                                           InOuts outOfStepBefore, WindowMain *parent) :
    mDialogCommandSelection(dialogCommandSelection),
    mParent(parent), mPipelineStepBefore(pipelineStepBefore), mSettings(settings), pipelineStepSettingsUi(pipelineStepSettingsUiIn),
    mOutOfStepBefore(outOfStepBefore)
{
  setObjectName("addCommandButton");
  setContentsMargins(0, 0, 0, 0);
  setFixedHeight(10);
}

void AddCommandButtonBase::paintEvent(QPaintEvent * /*event*/)
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

void AddCommandButtonBase::mousePressEvent(QMouseEvent * /*event*/)
{
  mDialogCommandSelection->show(mPipelineStepBefore, mOutOfStepBefore, &mSettings, pipelineStepSettingsUi);
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
}

}    // namespace joda::ui::gui
