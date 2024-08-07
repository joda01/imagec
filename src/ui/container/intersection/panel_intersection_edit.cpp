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
#include "../container_function.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/window_main/window_main.hpp"
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
  auto [horizontalLayout, _] = helper::createLayout(this, helper::SPACING);

  auto [verticalLayoutContainer, _1] = helper::addVerticalPanel(
      horizontalLayout, "rgba(218, 226, 255,0)", 0, false, helper::PANEL_WIDTH, helper::PANEL_WIDTH, helper::SPACING);
  auto [verticalLayoutMeta, _2] = helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
  verticalLayoutMeta->addWidget(helper::createTitle("Meta"));
  verticalLayoutMeta->addWidget(mParentContainer->mChannelName->getEditableWidget());
  verticalLayoutMeta->addWidget(mParentContainer->mColorAndChannelIndex->getEditableWidget());
  _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  connect(mParentContainer->mColorAndChannelIndex.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelIntersectionEdit::updatePreview);

  //
  // Cross channel
  //
  auto [llayoutColoc, _11] = helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
  llayoutColoc->addWidget(helper::createTitle("Cross-Channel"));
  llayoutColoc->addWidget(mParentContainer->mCrossChannelIntensity->getEditableWidget());
  llayoutColoc->addWidget(mParentContainer->mCrossChannelCount->getEditableWidget());
  _11->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  //
  // Measurement
  //
  // auto [verticalLayoutFuctions, _8] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)", 16, false);

  verticalLayoutContainer->addStretch(0);

  auto [detectionContainer, _4] = helper::addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false,
                                                           helper::PANEL_WIDTH, helper::PANEL_WIDTH, helper::SPACING);

  auto [verticalLayoutFilter, _6] =
      helper::addVerticalPanel(detectionContainer, "rgb(246, 246, 246)", helper::SPACING, false);
  verticalLayoutFilter->addWidget(helper::createTitle("Intersection"));
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
