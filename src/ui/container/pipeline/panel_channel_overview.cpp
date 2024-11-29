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

///

#include "panel_channel_overview.hpp"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include "ui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

PanelChannelOverview::PanelChannelOverview(WindowMain *wm, PanelPipelineSettings *parent) : mWindowMain(wm), mParentContainer(parent)
{
  setObjectName("PanelChannelOverview");
  setContentsMargins(0, 4, 4, 4);
  QGridLayout *layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
  layout->setSpacing(4);

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

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelChannelOverview::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    mWindowMain->showPanelPipelineSettingsEdit(mParentContainer);
  }
}

}    // namespace joda::ui
