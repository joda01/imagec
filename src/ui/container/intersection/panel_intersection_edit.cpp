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

#include "panel_intersection_edit.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <exception>
#include <memory>
#include <mutex>
#include <thread>
#include "../../window_main.hpp"
#include "../container_function.hpp"
#include "container_intersection.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

PanelIntersectionEdit::PanelIntersectionEdit(WindowMain *wm, ContainerIntersection *parentContainer) :
    mWindowMain(wm), mParentContainer(parentContainer)
{
  setObjectName("PanelIntersectionEdit");
  init();
}

void PanelIntersectionEdit::init()
{
  auto *horizontalLayout = createLayout();

  auto [verticalLayoutContainer, _1] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);
  auto [verticalLayoutMeta, _2]      = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
  verticalLayoutMeta->addWidget(createTitle("Meta"));
  verticalLayoutMeta->addWidget(mParentContainer->mChannelName->getEditableWidget());
  verticalLayoutMeta->addWidget(mParentContainer->mColorAndChannelIndex->getEditableWidget());
  _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  connect(mParentContainer->mColorAndChannelIndex.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelIntersectionEdit::updatePreview);

  //
  // Cross channel
  //
  auto [llayoutColoc, _11] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
  llayoutColoc->addWidget(createTitle("Cross-Channel"));
  llayoutColoc->addWidget(mParentContainer->mCrossChannelIntensity->getEditableWidget());
  llayoutColoc->addWidget(mParentContainer->mCrossChannelCount->getEditableWidget());
  _11->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  //
  // Measurement
  //
  // auto [verticalLayoutFuctions, _8] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)", 16, false);

  verticalLayoutContainer->addStretch(0);

  auto [detectionContainer, _4] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);

  auto [verticalLayoutFilter, _6] = addVerticalPanel(detectionContainer, "rgb(246, 246, 246)", 16, false);
  verticalLayoutFilter->addWidget(createTitle("Intersection"));
  verticalLayoutFilter->addWidget(mParentContainer->mCrossChannelIntersection->getEditableWidget());
  verticalLayoutFilter->addWidget(mParentContainer->mMinIntersection->getEditableWidget());

  verticalLayoutFilter->addStretch();
  _6->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  //
  // Preprocessing
  //

  // auto [functionContainer, _7]      = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);

  //_7->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

  /*
    //
    // Preview
    //
    auto [preview, _9] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, PREVIEW_BASE_SIZE);
    mPreviewImage      = new PanelPreview(PREVIEW_BASE_SIZE, PREVIEW_BASE_SIZE, this);
    mPreviewImage->resetImage("");
    preview->addWidget(mPreviewImage);
    QWidget *imageSubTitleWidget = new QWidget();
    imageSubTitleWidget->setMinimumHeight(50);
    QHBoxLayout *imageSubTitle = new QHBoxLayout();
    imageSubTitleWidget->setLayout(imageSubTitle);

    mSpinner = new WaitingSpinnerWidget(imageSubTitleWidget);
    mSpinner->setRoundness(10.0);
    mSpinner->setMinimumTrailOpacity(15.0);
    mSpinner->setTrailFadePercentage(70.0);
    mSpinner->setNumberOfLines(8);
    mSpinner->setLineLength(5);
    mSpinner->setLineWidth(2);
    mSpinner->setInnerRadius(5);
    mSpinner->setRevolutionsPerSecond(1);
    mSpinner->start();    // gets the show on the road!

    //
    // Signals from extern
    //
    connect(this, &PanelIntersectionEdit::updatePreviewStarted, mSpinner, &WaitingSpinnerWidget::start);
    connect(this, &PanelIntersectionEdit::updatePreviewFinished, mSpinner, &WaitingSpinnerWidget::stop);

    imageSubTitle->addWidget(mSpinner);

    imageSubTitle->addStretch(1);

    preview->addWidget(imageSubTitleWidget);
    */

  setLayout(horizontalLayout);
  horizontalLayout->addStretch();

  //
  // Signals from extern
  //
  connect(mWindowMain->getFoundFilesCombo(), &QComboBox::currentIndexChanged, this,
          &PanelIntersectionEdit::updatePreview);
  connect(mWindowMain->getImageSeriesCombo(), &QComboBox::currentIndexChanged, this,
          &PanelIntersectionEdit::updatePreview);
}

PanelIntersectionEdit::~PanelIntersectionEdit()
{
  {
    std::lock_guard<std::mutex> lock(mPreviewMutex);
    mPreviewCounter = 0;
  }
  if(mPreviewThread != nullptr) {
    if(mPreviewThread->joinable()) {
      mPreviewThread->join();
    }
  }
  delete mPreviewImage;
  delete mSpinner;
}

QLabel *PanelIntersectionEdit::createTitle(const QString &title)
{
  auto *label = new QLabel();
  QFont font;
  font.setPixelSize(16);
  font.setBold(true);
  label->setFont(font);
  label->setText(title);

  return label;
}

QHBoxLayout *PanelIntersectionEdit::createLayout()
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setObjectName("scrollArea");
  scrollArea->setFrameStyle(0);
  //   scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->verticalScrollBar()->setObjectName("scrollAreaV");

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  //   horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);
  return horizontalLayout;
}

std::tuple<QVBoxLayout *, QWidget *> PanelIntersectionEdit::addVerticalPanel(QLayout *horizontalLayout,
                                                                             const QString &bgColor, int margin,
                                                                             bool enableScrolling, int maxWidth,
                                                                             int spacing) const
{
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(spacing);
  QWidget *contentWidget = new QWidget();

  //   layout->setContentsMargins(margin, margin, margin, margin);
  layout->setAlignment(Qt::AlignTop);

  contentWidget->setObjectName("verticalContentChannel");
  contentWidget->setLayout(layout);
  contentWidget->setStyleSheet(
      "QWidget#verticalContentChannel {"
      "background-color: " +
      bgColor + ";}");

  if(enableScrolling) {
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setFrameStyle(0);
    //     scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->verticalScrollBar()->setObjectName("scrollAreaV");

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

void PanelIntersectionEdit::onCellApproximationChanged()
{
  updatePreview();
}

void PanelIntersectionEdit::onChannelTypeChanged()
{
  updatePreview();
}

void PanelIntersectionEdit::updatePreview()
{
}

}    // namespace joda::ui::qt
