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
  setStyleSheet(
      "QWidget#panelChannel { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: rgba(0, 104, 117, 0.05);}");
  //
  //  Header
  //
  // auto *header       = new QWidget();
  // auto *headerLayout = new QVBoxLayout(header);
  // headerLayout->setContentsMargins(0, 0, 0, 0);
  // header->setLayout(headerLayout);
  // headerLayout->addWidget(new PanelFunction());

  // layout->addWidget(header);
  // #dae2ff
  //
  // Body
  //
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setObjectName("scrollArea");
  scrollArea->setStyleSheet("QScrollArea#scrollArea { background-color: rgba(0, 0, 0, 0);}");
  scrollArea->setFrameStyle(0);
  scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->verticalScrollBar()->setStyleSheet(
      "QScrollBar:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    width: 6px;"
      "    margin: 0px 0px 0px 0px;"
      "}"
      "QScrollBar::handle:vertical {"
      "    background: rgba(32, 27, 23, 0.6);"
      "    min-height: 20px;"
      "    border-radius: 12px;"
      "}"
      "QScrollBar::add-line:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    height: 20px;"
      "    subcontrol-position: bottom;"
      "    subcontrol-origin: margin;"
      "}"
      "QScrollBar::sub-line:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    height: 20px;"
      "    subcontrol-position: top;"
      "    subcontrol-origin: margin;"
      "}");
  layout->addWidget(scrollArea);
  setLayout(layout);

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("verticalLayout");
  contentWidget->setStyleSheet("QWidget#verticalLayout { background-color: rgba(0, 104, 117, 0);}");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QVBoxLayout *verticalLayout = new QVBoxLayout(contentWidget);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  contentWidget->setLayout(verticalLayout);

  {
    PanelFunction *widget = new PanelFunction("icons8-text-50.png", "...", "Channel name", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }

  verticalLayout->addWidget(new QLabel(" "));

  {
    PanelFunction *widget = new PanelFunction("icons8-bubble-50.png", "[0-50]", "Rolling ball", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }
  {
    PanelFunction *widget = new PanelFunction("icons8-crop-50.png", "[0-65535]", "Margin Crop", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }
  {
    PanelFunction *widget = new PanelFunction("icons8-blur-50.png", "[0-4]", "Smoothing", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }
  {
    PanelFunction *widget =
        new PanelFunction("icons8-background-remover-50.png", "Channel Nr", "Background subtraction", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-ellipse-50.png", "[0-1]", "Circularity", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }
  {
    PanelFunction *widget = new PanelFunction("icons8-all-out-50.png", "[0-65536]", "Object size", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }
  {
    PanelFunction *widget = new PanelFunction("icons8-initial-state-50.png", "[0-65536]", "Snap area", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(widget);
  }

  verticalLayout->addStretch();
}

}    // namespace joda::ui::qt
