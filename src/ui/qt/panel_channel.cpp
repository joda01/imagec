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

#include "panel_channel.hpp"
#include <qlabel.h>
#include <qlineedit.h>
#include "panel_function.hpp"

namespace joda::ui::qt {

PanelChannel::PanelChannel()
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("panelChannel");
  setMinimumWidth(250);
  setMaximumWidth(250);
  QVBoxLayout *layout = new QVBoxLayout(this);
  setStyleSheet("QWidget#panelChannel {border: 1px solid black;  border-radius: 8px; }");
  //
  // Header
  //
  auto *header       = new QWidget();
  auto *headerLayout = new QVBoxLayout(header);
  headerLayout->setContentsMargins(0, 0, 0, 0);
  header->setLayout(headerLayout);
  headerLayout->addWidget(new PanelFunction());

  layout->addWidget(header);

  //
  // Body
  //
  QScrollArea *scrollArea = new QScrollArea(this);
  // scrollArea->setStyleSheet("QScrollArea { border: none; }");
  scrollArea->setFrameStyle(0);
  scrollArea->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(scrollArea);
  setLayout(layout);

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QVBoxLayout *verticalLayout = new QVBoxLayout(contentWidget);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  contentWidget->setLayout(verticalLayout);

  for(int i = 0; i < 20; ++i) {
    PanelFunction *widget = new PanelFunction(this);
    verticalLayout->addWidget(widget);
  }
}

}    // namespace joda::ui::qt
