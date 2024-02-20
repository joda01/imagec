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
#include "ui/qt/panel_label_overview.hpp"
#include "panel_function.hpp"
#include "window_main.hpp"

namespace joda::ui::qt {

PanelChannelOverview::PanelChannelOverview(WindowMain *wm) : mWindowMain(wm)
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
  layout->addWidget(new PanelLabelOverview("icons8-text-50.png", "Channel 1 (CY3)"), 0, 0, 1, 3);
  layout->addWidget(new PanelLabelOverview("icons8-lambda-50.png", "Li"), 1, 0);
  layout->addWidget(new PanelLabelOverview("icons8-grayscale-50.png", "1000"), 1, 1);
  layout->addWidget(new PanelLabelOverview("icons8-ellipse-50.png", "50 %"), 1, 2);
  layout->addWidget(new PanelLabelOverview("icons8-all-out-50.png", "0-65 px"), 2, 0);
  layout->addWidget(new PanelLabelOverview("icons8-initial-state-50.png", "0 px"), 2, 1);
  layout->addWidget(new PanelLabelOverview("icons8-layers-50.png", "z-project"), 2, 2);
  layout->addWidget(new PanelLabelOverview("icons8-crop-50.png", "0 px"), 3, 0);
  layout->addWidget(new PanelLabelOverview("icons8-sheets-50.png", "ON"), 3, 1);
  layout->addWidget(new PanelLabelOverview("icons8-baseline-50.png", "OFF"), 3, 2);
  layout->addWidget(new PanelLabelOverview("icons8-bubble-50.png", "4 px"), 4, 0);
  layout->addWidget(new PanelLabelOverview("icons8-blur-50.png", "4 px"), 4, 1);
  layout->addWidget(new PanelLabelOverview("icons8-cleanup-noise-50.png", "OFF"), 4, 2);
  layout->addWidget(new PanelLabelOverview("icons8-triangle-50.png", "OFF"), 5, 0);
  layout->addWidget(new PanelLabelOverview("icons8-final-state-50.png", "OFF"), 5, 1);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void PanelChannelOverview::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // Left mouse button clicked
    qDebug() << "Widget Clicked!";
    mWindowMain->showChannelEdit();
  }
}

}    // namespace joda::ui::qt
