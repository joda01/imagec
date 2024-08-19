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

#include "panel_giraf_overview.hpp"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwidget.h>
#include "../container_function.hpp"
#include "ui/window_main/window_main.hpp"
#include "container_giraf.hpp"

namespace joda::ui {

PanelGirafOverview::PanelGirafOverview(WindowMain *wm, ContainerGiraf *parent) :
    mWindowMain(wm), mParentContainer(parent)
{
  setObjectName("PanelChannelOverview");
  setMinimumWidth(350);
  setMaximumWidth(350);
  setMinimumHeight(250);
  setMaximumHeight(350);
  QVBoxLayout *layout = new QVBoxLayout();

  setLayout(layout);
  layout->setSpacing(0);
  //   layout->setContentsMargins(28, 28, 28, 28);

  QMovie *mGiraf = new QMovie(":/icons/outlined/girafa.gif");
  mGiraf->setScaledSize(QSize(200, 200));

  QLabel *q = new QLabel(this);
  q->setMovie(mGiraf);
  layout->addWidget(q);
  mGiraf->start();

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelGirafOverview::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelGirafOverview::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    mWindowMain->showPanelPipelineSettingsEdit(mParentContainer);
  }
}

}    // namespace joda::ui
