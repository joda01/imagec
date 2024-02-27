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

  auto [verticalLayoutContainer, _1] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto [verticalLayoutMeta, _2]      = addVerticalPanel(verticalLayoutContainer, "rgba(0, 104, 117, 0.05)");
  verticalLayoutMeta->addWidget(createTitle("Meta"));
  verticalLayoutMeta->addWidget(parentContainer->mChannelName->getEditableWidget());
  verticalLayoutMeta->addWidget(parentContainer->mChannelIndex->getEditableWidget());
  verticalLayoutMeta->addWidget(parentContainer->mChannelType->getEditableWidget());
  _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  connect(parentContainer->mChannelType.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::onChannelTypeChanged);

  auto [layoutCellApproximation, _3] = addVerticalPanel(verticalLayoutContainer, "rgba(0, 104, 117, 0.05)");
  mScrollAreaCellApprox              = _3;
  layoutCellApproximation->addWidget(createTitle("Cell approximation"));
  layoutCellApproximation->addWidget(parentContainer->mEnableCellApproximation->getEditableWidget());
  layoutCellApproximation->addWidget(parentContainer->mMaxCellRadius->getEditableWidget());
  mParentContainer->mMaxCellRadius->getEditableWidget()->setVisible(false);
  _3->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  connect(parentContainer->mEnableCellApproximation.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::onCellApproximationChanged);

  verticalLayoutContainer->addStretch(0);

  mScrollAreaCellApprox->setVisible(false);

  auto [detectionContainer, _4] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto [detection, _5]          = addVerticalPanel(detectionContainer, "rgba(0, 104, 117, 0.05)");

  detection->addWidget(createTitle("Detection"));
  detection->addWidget(parentContainer->mUsedDetectionMode->getEditableWidget());
  connect(parentContainer->mUsedDetectionMode.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::onDetectionModechanged);
  detection->addWidget(parentContainer->mThresholdAlgorithm->getEditableWidget());
  detection->addWidget(parentContainer->mThresholdValueMin->getEditableWidget());
  detection->addWidget(parentContainer->mAIModels->getEditableWidget());
  detection->addWidget(parentContainer->mMinProbability->getEditableWidget());
  _5->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  onDetectionModechanged();

  auto [verticalLayoutFilter, _6] = addVerticalPanel(detectionContainer, "rgba(0, 104, 117, 0.05)", 16, false);
  verticalLayoutFilter->addWidget(createTitle("Filtering"));
  verticalLayoutFilter->addWidget(parentContainer->mMinParticleSize->getEditableWidget());
  verticalLayoutFilter->addWidget(parentContainer->mMaxParticleSize->getEditableWidget());
  verticalLayoutFilter->addWidget(parentContainer->mMinCircularity->getEditableWidget());
  verticalLayoutFilter->addWidget(parentContainer->mSnapAreaSize->getEditableWidget());
  verticalLayoutFilter->addWidget(parentContainer->mTetraspeckRemoval->getEditableWidget());
  verticalLayoutFilter->addStretch();
  _6->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  auto [functionContainer, _7]      = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto [verticalLayoutFuctions, _8] = addVerticalPanel(functionContainer, "rgba(0, 104, 117, 0.05)", 16, false);
  verticalLayoutFuctions->addWidget(createTitle("Preprocessing"));
  verticalLayoutFuctions->addWidget(parentContainer->mZProjection->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mMarginCrop->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mMedianBackgroundSubtraction->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mEdgeDetection->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mRollingBall->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mSubtractChannel->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mSmoothing->getEditableWidget());
  verticalLayoutFuctions->addWidget(parentContainer->mGaussianBlur->getEditableWidget());
  _8->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  _7->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

  //
  // Preview
  //
  auto [preview, _9] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 500);
  mPreviewImage      = new QLabel("...");

  // Load the image using QPixmap
  QIcon bmp(":/icons/outlined/placeholder_view_vector.svg.png");

  // Set the image on the label
  mPreviewImage->setPixmap(bmp.pixmap(350, 350));

  preview->addWidget(mPreviewImage);

  setLayout(horizontalLayout);
  horizontalLayout->addStretch();
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

std::tuple<QVBoxLayout *, QWidget *> PanelChannelEdit::addVerticalPanel(QLayout *horizontalLayout,
                                                                        const QString &bgColor, int margin,
                                                                        bool enableScrolling, int maxWidth) const
{
  QVBoxLayout *layout    = new QVBoxLayout();
  QWidget *contentWidget = new QWidget();

  layout->setContentsMargins(margin, margin, margin, margin);
  layout->setAlignment(Qt::AlignTop);

  contentWidget->setObjectName("verticalContentChannel");
  contentWidget->setLayout(layout);
  contentWidget->setStyleSheet(
      "QWidget#verticalContentChannel { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: " +
      bgColor + ";}");

  if(enableScrolling) {
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
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

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumWidth(maxWidth);
    scrollArea->setMaximumWidth(maxWidth);

    horizontalLayout->addWidget(scrollArea);
    return {layout, scrollArea};
  }
  contentWidget->setMinimumWidth(maxWidth);
  contentWidget->setMaximumWidth(maxWidth);
  horizontalLayout->addWidget(contentWidget);

  return {layout, contentWidget};
}

void PanelChannelEdit::onCellApproximationChanged()
{
  mParentContainer->mMaxCellRadius->getEditableWidget()->setVisible(
      mParentContainer->mEnableCellApproximation->getValue());
}

void PanelChannelEdit::onChannelTypeChanged()
{
  if(mParentContainer->mChannelType->getValue() == "NUCLEUS") {
    mScrollAreaCellApprox->setVisible(true);
  } else {
    mScrollAreaCellApprox->setVisible(false);
  }
}

void PanelChannelEdit::onDetectionModechanged()
{
  if(mParentContainer->mUsedDetectionMode->getValue() == "AI") {
    mParentContainer->mMinProbability->getEditableWidget()->setVisible(true);
    mParentContainer->mAIModels->getEditableWidget()->setVisible(true);

    mParentContainer->mThresholdAlgorithm->getEditableWidget()->setVisible(false);
    mParentContainer->mThresholdValueMin->getEditableWidget()->setVisible(false);

  } else {
    mParentContainer->mMinProbability->getEditableWidget()->setVisible(false);
    mParentContainer->mAIModels->getEditableWidget()->setVisible(false);

    mParentContainer->mThresholdAlgorithm->getEditableWidget()->setVisible(true);
    mParentContainer->mThresholdValueMin->getEditableWidget()->setVisible(true);
  }
}

}    // namespace joda::ui::qt
