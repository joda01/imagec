///
/// \file      panel_channel.cpp
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

#include "panel_channel_overview.hpp"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include "../../window_main.hpp"
#include "../container_function.hpp"
#include "container_channel.hpp"

namespace joda::ui::qt {

PanelChannelOverview::PanelChannelOverview(WindowMain *wm, ContainerChannel *parent) :
    mWindowMain(wm), mParentContainer(parent)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelChannelOverview");
  setMinimumWidth(350);
  setMaximumWidth(350);
  QGridLayout *layout = new QGridLayout(this);
  setStyleSheet(
      "QWidget#PanelChannelOverview { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: rgb(246, 246, 246);}");

  setLayout(layout);
  layout->setSpacing(0);

  // Add the functions
  layout->addWidget(parent->mChannelName->getLabelWidget(), 0, 0, 1, 3);

  layout->addWidget(parent->mColorAndChannelIndex->getLabelWidget(), 1, 0);
  layout->addWidget(parent->mChannelType->getLabelWidget(), 1, 1);
  layout->addWidget(parent->mThresholdAlgorithm->getLabelWidget(), 1, 2);

  layout->addWidget(parent->mThresholdValueMin->getLabelWidget(), 2, 0);
  layout->addWidget(parent->mMinCircularity->getLabelWidget(), 2, 1);
  layout->addWidget(parent->mMinParticleSize->getLabelWidget(), 2, 2);

  layout->addWidget(parent->mSnapAreaSize->getLabelWidget(), 3, 0);
  layout->addWidget(parent->mZProjection->getLabelWidget(), 3, 1);
  layout->addWidget(parent->mMarginCrop->getLabelWidget(), 3, 2);

  layout->addWidget(parent->mSubtractChannel->getLabelWidget(), 4, 0);
  layout->addWidget(parent->mMedianBackgroundSubtraction->getLabelWidget(), 4, 1);
  layout->addWidget(parent->mRollingBall->getLabelWidget(), 4, 2);

  layout->addWidget(parent->mGaussianBlur->getLabelWidget(), 5, 0);
  layout->addWidget(parent->mSmoothing->getLabelWidget(), 5, 1);
  layout->addWidget(parent->mEdgeDetection->getLabelWidget(), 5, 2);

  layout->addWidget(parent->mTetraspeckRemoval->getLabelWidget(), 6, 0);
  layout->addWidget(parent->mCrossChannelIntensity->getLabelWidget(), 6, 1);
  layout->addWidget(parent->mCrossChannelCount->getLabelWidget(), 6, 2);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelChannelOverview::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    mWindowMain->showChannelEdit(mParentContainer);
  }
}

}    // namespace joda::ui::qt
