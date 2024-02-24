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

#include "panel_channel_edit.hpp"
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include "container_channel.hpp"
#include "container_function.hpp"
#include "window_main.hpp"

namespace joda::ui::qt {

PanelChannelEdit::PanelChannelEdit(WindowMain *wm, ContainerChannel *parentContainer) :
    mWindowMain(wm), mParentContainer(parentContainer)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelChannelEdit");

  auto *horizontalLayout = createLayout();

  auto *verticalLayoutContainer = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto *verticalLayoutMeta      = addVerticalPanel(verticalLayoutContainer, "rgba(0, 104, 117, 0.05)");

  verticalLayoutMeta->addWidget(createTitle("Meta"));
  verticalLayoutMeta->addWidget(parentContainer->mChannelName->getEditableWidget());
  verticalLayoutMeta->addWidget(parentContainer->mChannelIndex->getEditableWidget());
  verticalLayoutMeta->addWidget(parentContainer->mChannelType->getEditableWidget());

  auto *verticalLayoutPreview = addVerticalPanel(verticalLayoutContainer, "rgba(0, 104, 117, 0.05)");

  QPushButton *remove = new QPushButton("Remove");
  connect(remove, &QPushButton::pressed, this, &PanelChannelEdit::onRemoveClicked);
  verticalLayoutPreview->addWidget(remove);

  verticalLayoutMeta->addStretch();
  verticalLayoutContainer->addStretch();
  verticalLayoutPreview->addStretch();

  auto *detectionContainer = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto *detection          = addVerticalPanel(detectionContainer, "rgba(0, 104, 117, 0.05)");

  detection->addWidget(createTitle("Detection"));
  detection->addWidget(parentContainer->mThresholdAlgorithm->getEditableWidget());
  detection->addWidget(parentContainer->mThresholdValueMin->getEditableWidget());

  auto *verticalLayoutFilter = addVerticalPanel(detectionContainer, "rgba(0, 104, 117, 0.05)");
  verticalLayoutFilter->addWidget(createTitle("Filtering"));
  verticalLayoutFilter->addWidget(parentContainer->mMinCircularity->getEditableWidget());
  verticalLayoutFilter->addWidget(parentContainer->mSnapAreaSize->getEditableWidget());
  verticalLayoutFilter->addWidget(parentContainer->mTetraspeckRemoval->getEditableWidget());

  verticalLayoutFilter->addStretch();
  detection->addStretch();
  detectionContainer->addStretch();

  auto *functionContainer = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);

  auto *verticalLayoutFuctions = addVerticalPanel(functionContainer, "rgba(0, 104, 117, 0.05)", 16, false);

  verticalLayoutFuctions->addWidget(createTitle("Preprocessing"));
  verticalLayoutFuctions->addWidget(parentContainer->mZProjection->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mMarginCrop->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mSubtractChannel->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mMedianBackgroundSubtraction->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mRollingBall->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mGaussianBlur->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mSmoothing->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mEdgeDetection->getEditableWidget());

  verticalLayoutFuctions->addStretch();
  // Comment out
  // functionContainer->addStretch();

  horizontalLayout->addStretch();
  setLayout(horizontalLayout);
}

QLabel *PanelChannelEdit::createTitle(const QString &title)
{
  auto *label = new QLabel();
  QFont font;
  font.setPixelSize(16);
  font.setBold(true);
  label->setFont(font);
  label->setText(title);

  return label;
}

QHBoxLayout *PanelChannelEdit::createLayout()
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

QVBoxLayout *PanelChannelEdit::addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor, int margin,
                                                bool enableScrolling) const
{
  QScrollArea *scrollArea = new QScrollArea();
  if(!enableScrolling) {
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  }
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

void PanelChannelEdit::onRemoveClicked()
{
  mWindowMain->removeChannel(this->mParentContainer);
}

}    // namespace joda::ui::qt
