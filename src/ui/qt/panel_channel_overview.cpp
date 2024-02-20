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
#include <qlabel.h>
#include <qlineedit.h>
#include "panel_function.hpp"

namespace joda::ui::qt {

PanelChannelOverview::PanelChannelOverview()
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelChannelOverview");
  setMinimumWidth(250);
  setMaximumWidth(250);
  QVBoxLayout *layout = new QVBoxLayout(this);
  setStyleSheet(
      "QWidget#PanelChannelOverview { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: rgba(0, 104, 117, 0.05);}");

  setLayout(layout);

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("verticalLayoutOverview");
  contentWidget->setStyleSheet("QWidget#verticalLayoutOverview { background-color: rgba(0, 104, 117, 0);}");

  // Create a horizontal layout for the panels
  QVBoxLayout *verticalLayout = new QVBoxLayout(contentWidget);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  contentWidget->setLayout(verticalLayout);

  {
    PanelFunction *widget = new PanelFunction("icons8-text-50.png", "...", "Channel name", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }
  verticalLayout->addStretch();
  layout->addLayout(verticalLayout);
}

}    // namespace joda::ui::qt
