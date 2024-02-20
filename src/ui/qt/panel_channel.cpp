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
#include <qpushbutton.h>
#include "panel_function.hpp"
#include "window_main.hpp"

namespace joda::ui::qt {

PanelChannel::PanelChannel(WindowMain *wm) : mWindowMain(wm)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("panelChannel");

  auto *horizontalLayout = createLayout();

  auto *verticalLayoutContainer = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto *verticalLayoutMeta      = addVerticalPanel(verticalLayoutContainer, "rgba(0, 104, 117, 0.05)");

  {
    PanelFunction *widget = new PanelFunction("icons8-text-50.png", "...", "Channel name", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutMeta->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-unknown-status-50.png", "...", "Channel type", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutMeta->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-sheets-50.png", "0,1,2,3", "Channel index", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutMeta->addWidget(widget);
  }

  auto *verticalLayoutPreview = addVerticalPanel(verticalLayoutContainer, "rgba(0, 104, 117, 0.05)");

  QPushButton *back = new QPushButton("Back");
  connect(back, &QPushButton::pressed, this, &PanelChannel::onBackClicked);
  verticalLayoutPreview->addWidget(back);

  QPushButton *remove = new QPushButton("Remove");
  connect(remove, &QPushButton::pressed, this, &PanelChannel::onRemoveClicked);
  verticalLayoutPreview->addWidget(remove);

  verticalLayoutMeta->addStretch();
  verticalLayoutContainer->addStretch();
  verticalLayoutPreview->addStretch();

  auto *detectionContainer = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto *detection          = addVerticalPanel(detectionContainer, "rgba(0, 104, 117, 0.05)");

  {
    PanelFunction *widget = new PanelFunction("icons8-lambda-50.png", "[0-50]", "Threshold algorithm", this);
    widget->setContentsMargins(0, 0, 0, 0);
    detection->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-grayscale-50.png", "[0-50]", "Min. threshold", this);
    widget->setContentsMargins(0, 0, 0, 0);
    detection->addWidget(widget);
  }

  auto *verticalLayoutFilter = addVerticalPanel(detectionContainer, "rgba(0, 104, 117, 0.05)");

  {
    PanelFunction *widget = new PanelFunction("icons8-ellipse-50.png", "[0-1]", "Min. Circularity", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFilter->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-all-out-50.png", "0 - 65536", "Size range", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFilter->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-initial-state-50.png", "[0-65535]", "Snap area size", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFilter->addWidget(widget);
  }
  verticalLayoutFilter->addStretch();
  detection->addStretch();
  detectionContainer->addStretch();

  auto *functionContainer = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);

  auto *verticalLayoutFuctions = addVerticalPanel(functionContainer, "rgba(0, 104, 117, 0.05)");

  {
    PanelFunction *widget = new PanelFunction("icons8-layers-50.png", "[0-50]", "Z-Projection", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-crop-50.png", "[0-50]", "Margin crop", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-sheets-50.png", "[0,1,2]", "Subtract channel", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }
  {
    PanelFunction *widget = new PanelFunction("icons8-baseline-50.png", "[0-4]", "Median bg. subtraction", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-bubble-50.png", "[0-50]", "Rolling ball", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }
  {
    PanelFunction *widget = new PanelFunction("icons8-blur-50.png", "[0-4]", "Gaussian blur", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }
  {
    PanelFunction *widget = new PanelFunction("icons8-cleanup-noise-50.png", "[0-4]", "Smoothing", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-triangle-50.png", "[0-4]", "Edge detection", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }

  {
    PanelFunction *widget = new PanelFunction("icons8-final-state-50.png", "[0-4]", "Tetraspeck removal", this);
    widget->setContentsMargins(0, 0, 0, 0);
    verticalLayoutFuctions->addWidget(widget);
  }

  verticalLayoutFuctions->addStretch();
  // functionContainer->addStretch();

  horizontalLayout->addStretch();
  setLayout(horizontalLayout);
}

QHBoxLayout *PanelChannel::createLayout()
{
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

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");
  contentWidget->setStyleSheet("QWidget#contentOverview { background-color: rgb(251, 252, 253);}");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);
  return horizontalLayout;
}

QVBoxLayout *PanelChannel::addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor, int margin) const
{
  QScrollArea *scrollArea = new QScrollArea();
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

  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("verticalContentChannel");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  QVBoxLayout *layout = new QVBoxLayout(contentWidget);
  layout->setContentsMargins(margin, margin, margin, margin);
  contentWidget->setLayout(layout);
  horizontalLayout->addWidget(scrollArea);

  contentWidget->setStyleSheet(
      "QWidget#verticalContentChannel { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: " +
      bgColor + ";}");

  scrollArea->setMinimumWidth(250);
  scrollArea->setMaximumWidth(250);

  return layout;
}

void PanelChannel::onBackClicked()
{
  mWindowMain->showOverview();
}

void PanelChannel::onRemoveClicked()
{
  mWindowMain->removeChannel();
  mWindowMain->showOverview();
}

}    // namespace joda::ui::qt
