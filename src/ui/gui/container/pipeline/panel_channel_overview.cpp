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
  // Handel for movee
  //
  {
    int x          = width() - HANDLE_WITH;
    int ovalHeight = 2;    //(height() - 8 - 8) / 3;
    painter.setBrush(Qt::lightGray);
    QPen pen(Qt::lightGray, 1);    // darkYellow, 5px width
    painter.setPen(pen);
    painter.drawEllipse(x, 4, ovalHeight, ovalHeight);
    painter.drawEllipse(x, ovalHeight + 8, ovalHeight, ovalHeight);
    painter.drawEllipse(x, ovalHeight * 2 + 8 + 4, ovalHeight, ovalHeight);
  }

  //
  // Paint if disables
  //
  if((mParentContainer != nullptr) && (mParentContainer->mActionDisabled != nullptr) && mParentContainer->mActionDisabled->isChecked()) {
    painter.setBrush(Qt::red);
    QPen pen(Qt::red, 1);    // darkYellow, 5px width
    painter.drawEllipse(2, height() / 2 - 3, 6, 6);
  } else {
    painter.setBrush(Qt::green);
    QPen pen(Qt::green, 1);    // darkYellow, 5px width
  }

  QWidget::paintEvent(event);    // Call the base class paint event
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelChannelOverview::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && event->position().x() >= (size().width() - HANDLE_WITH - 2)) {
    startDrag();
  }

  QWidget::mousePressEvent(event);
}

void PanelChannelOverview::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    mWindowMain->showPanelPipelineSettingsEdit(mParentContainer);
  }
}

void PanelChannelOverview::startDrag()
{
  QDrag *drag         = new QDrag(this);
  QMimeData *mimeData = new QMimeData;
  mimeData->setText(QString::number((qintptr) this));    // Store the widget's address as text
  drag->setMimeData(mimeData);

  // Create a pixmap for the drag image (optional, but recommended)
  QPixmap pixmap(size());                     // Create a pixmap of the widget's size
  pixmap.fill(QColor(200, 200, 200, 192));    // Fill it with gray

  QPainter painter(&pixmap);
  render(&painter);    // Render the widget onto the pixmap
  painter.end();

  // Calculate the offset between the mouse click and the widget's center
  QPoint mousePosInWidget = mapFromGlobal(QCursor::pos());
  QPoint offset           = mousePosInWidget - QPoint(width() / 2, height() / 2);

  drag->setPixmap(pixmap);
  drag->setHotSpot(QPoint(pixmap.width() / 2 + offset.x(), pixmap.height() / 2));    // Center the hotspot

  drag->exec(Qt::CopyAction | Qt::MoveAction);
}

}    // namespace joda::ui::gui
