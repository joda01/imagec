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

#include "panel_giraf_overview.hpp"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwidget.h>
#include "../../window_main.hpp"
#include "../container_function.hpp"
#include "container_giraf.hpp"

namespace joda::ui::qt {

PanelGirafOverview::PanelGirafOverview(WindowMain *wm, ContainerGiraf *parent) :
    mWindowMain(wm), mParentContainer(parent)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelGirafOverview");
  setMinimumWidth(350);
  setMaximumWidth(350);
  setMinimumHeight(250);
  setMaximumHeight(350);
  QVBoxLayout *layout = new QVBoxLayout();
  setStyleSheet(
      "QWidget#PanelGirafOverview { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: rgb(246, 246, 246);}");

  setLayout(layout);
  layout->setSpacing(0);
  layout->setContentsMargins(28, 28, 28, 28);

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
    mWindowMain->showChannelEdit(mParentContainer);
  }
}

}    // namespace joda::ui::qt