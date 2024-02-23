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
#include "container_channel.hpp"
#include "container_function.hpp"
#include "window_main.hpp"

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
      "background-color: rgba(0, 104, 117, 0.05);}");

  setLayout(layout);
  layout->setSpacing(0);

  // Add the functions
  layout->addWidget(parent->mChannelName->getLabelWidget(), 0, 0, 1, 3);
  layout->addWidget(parent->mChannelSelector->getLabelWidget(), 1, 0);

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
