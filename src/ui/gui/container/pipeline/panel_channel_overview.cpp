///
/// \file      panel_channel.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_channel_overview.hpp"
#include <qcolor.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include "ui/gui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

PanelChannelOverview::PanelChannelOverview(WindowMain *wm, PanelPipelineSettings *parent) : QWidget(parent), mWindowMain(wm), mParentContainer(parent)
{
  setObjectName("PanelChannelOverview");
  setContentsMargins(8, 4, 8, 4);
  QGridLayout *layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
  layout->setSpacing(4);

  // It should be droppable
  setAcceptDrops(false);

  // Add the functions
  // layout->addWidget(parent->mChannelName->getLabelWidget(), 0, 0, 1, 3);
  layout->addWidget(parent->pipelineName->getDisplayLabelWidget(), 0, 0);
  layout->addWidget(parent->defaultClassId->getDisplayLabelWidget(), 0, 1);
  layout->addWidget(parent->cStackIndex->getDisplayLabelWidget(), 0, 2);
  /*
    layout->addWidget(new QLabel(), 1, 0);
    layout->addWidget(new QLabel(), 1, 1);
    layout->addWidget(new QLabel(), 1, 2);

    layout->addWidget(new QLabel(), 2, 0);
    layout->addWidget(new QLabel(), 2, 1);
    layout->addWidget(new QLabel(), 2, 2);
  */
  /*
    layout->addWidget(parent->mSubtractChannel->getLabelWidget(), 4, 0);
    layout->addWidget(parent->mMedianBackgroundSubtraction->getLabelWidget(), 4, 1);
    layout->addWidget(parent->mRollingBall->getLabelWidget(), 4, 2);

    layout->addWidget(parent->mGaussianBlur->getLabelWidget(), 5, 0);
    layout->addWidget(parent->mSmoothing->getLabelWidget(), 5, 1);
    layout->addWidget(parent->mEdgeDetection->getLabelWidget(), 5, 2);

    layout->addWidget(parent->mTetraspeckRemoval->getLabelWidget(), 6, 0);
    layout->addWidget(parent->mCrossChannelIntensity->getLabelWidget(), 6, 1);
    layout->addWidget(parent->mCrossChannelCount->getLabelWidget(), 6, 2);
  */
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void PanelChannelOverview::paintEvent(QPaintEvent *event)
{
  //
  // Paint the handle on the right handside which allows to drag the pipeline step
  //
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  //
  // Paint if disables
  //
  if((mParentContainer != nullptr) && (mParentContainer->mActionDisabled != nullptr) && mParentContainer->mActionDisabled->isChecked()) {
    painter.setBrush(Qt::red);
    QPen pen(Qt::red, 1);    // darkYellow, 5px width
    painter.setPen(pen);
    // painter.drawEllipse(2, height() / 2 - 3, 6, 6);
    int crossWith = 8;
    int margin    = 2;
    int marginTop = 8;
    painter.drawLine(margin, marginTop, crossWith - margin, height() - marginTop);
    painter.drawLine(crossWith - margin, marginTop, margin, height() - marginTop);

  } else {
    painter.setBrush(Qt::green);
    QPen pen(Qt::green, 1);    // darkYellow, 5px width
  }

  QWidget::paintEvent(event);    // Call the base class paint event
}

void PanelChannelOverview::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    QMessageBox::information(mWindowMain, "Hint...", "One click is enough to open ;)");
  }
}

}    // namespace joda::ui::gui
