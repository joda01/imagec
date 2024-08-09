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

#include "panel_voronoi_overview.hpp"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include "../container_function.hpp"
#include "ui/window_main/window_main.hpp"
#include "container_voronoi.hpp"

namespace joda::ui::qt {

PanelVoronoiOverview::PanelVoronoiOverview(WindowMain *wm, ContainerVoronoi *parent) :
    mWindowMain(wm), mParentContainer(parent)
{
  setObjectName("PanelChannelOverview");
  // setMinimumWidth(320);
  //  setMaximumWidth(350);
  QGridLayout *layout = new QGridLayout(this);

  setLayout(layout);
  layout->setSpacing(0);

  // Add the functions
  // layout->addWidget(parent->mChannelName->getLabelWidget(), 0, 0, 1, 3);

  layout->addWidget(parent->mColorAndChannelIndex->getLabelWidget(), 0, 0);
  layout->addWidget(parent->mVoronoiPoints->getLabelWidget(), 0, 1);
  layout->addWidget(parent->mOverlayMaskChannelIndex->getLabelWidget(), 0, 2);

  layout->addWidget(parent->mCrossChannelIntensity->getLabelWidget(), 1, 0);
  layout->addWidget(parent->mCrossChannelCount->getLabelWidget(), 1, 1);
  layout->addWidget(parent->mMaxVoronoiAreaSize->getLabelWidget(), 1, 2);

  layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 2, 0);
  layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 2, 1);
  layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 2, 2);

  /* layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 4, 0);
   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 4, 1);
   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 4, 2);

   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 5, 0);
   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 5, 1);
   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 5, 2);

   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 6, 0);
   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 6, 1);
   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 6, 2);

   layout->addWidget(ContainerFunctionBase::createDisplayAbleWidgetPlaceholder(), 6, 3);*/

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelVoronoiOverview::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    mWindowMain->showChannelEdit(mParentContainer);
  }
}

}    // namespace joda::ui::qt
