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

#include "panel_voronoi_edit.hpp"
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
#include "container_voronoi.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

PanelVoronoiEdit::PanelVoronoiEdit(WindowMain *wm, ContainerVoronoi *parentContainer) :
    mWindowMain(wm), mParentContainer(parentContainer)
{
  setObjectName("PanelVoronoiEdit");
  init();
}

void PanelVoronoiEdit::init()
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
          &PanelVoronoiEdit::updatePreview);

  //
  // Cross channel
  //
  auto [llayoutColoc, _11] = helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
  llayoutColoc->addWidget(helper::createTitle("Cross-Channel"));
  // llayoutColoc->addWidget(parentContainer->mColocGroup->getEditableWidget());
  llayoutColoc->addWidget(mParentContainer->mCrossChannelIntensity->getEditableWidget());
  llayoutColoc->addWidget(mParentContainer->mCrossChannelCount->getEditableWidget());
  _11->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  //
  // Measurement
  //
  // auto [verticalLayoutFuctions, _8] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)", 16, false);

  // Column 2
  auto [detectionContainer, _4] = helper::addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false,
                                                           helper::PANEL_WIDTH, helper::PANEL_WIDTH, helper::SPACING);
  auto [verticalLayoutFilter, _6] =
      helper::addVerticalPanel(detectionContainer, "rgb(246, 246, 246)", helper::SPACING, false);
  verticalLayoutFilter->addWidget(helper::createTitle("Voronoi"));
  verticalLayoutFilter->addWidget(mParentContainer->mVoronoiPoints->getEditableWidget());
  verticalLayoutFilter->addWidget(mParentContainer->mMaxVoronoiAreaSize->getEditableWidget());
  verticalLayoutFilter->addWidget(mParentContainer->mOverlayMaskChannelIndex->getEditableWidget());

  verticalLayoutFilter->addStretch();
  _6->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  connect(mParentContainer->mMaxVoronoiAreaSize.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);
  connect(mParentContainer->mOverlayMaskChannelIndex.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);

  //
  // Column 4
  //
  auto [filterContainer, filterContainerLayout] = helper::addVerticalPanel(
      horizontalLayout, "rgba(218, 226, 255,0)", 0, false, helper::PANEL_WIDTH, helper::PANEL_WIDTH, helper::SPACING);
  auto [objectFilter, objectFilterLayout] = helper::addVerticalPanel(filterContainer, "rgb(246, 246, 246)");
  objectFilter->addWidget(helper::createTitle("Object filter"));
  objectFilter->addWidget(mParentContainer->mMinParticleSize->getEditableWidget());
  objectFilter->addWidget(mParentContainer->mMaxParticleSize->getEditableWidget());
  objectFilter->addWidget(mParentContainer->mExcludeAreasAtTheEdges->getEditableWidget());
  objectFilter->addWidget(mParentContainer->mExcludeAreasWithoutCenterOfMass->getEditableWidget());
  objectFilter->addStretch();
  objectFilterLayout->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  connect(mParentContainer->mMinParticleSize.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);
  connect(mParentContainer->mMaxParticleSize.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);
  connect(mParentContainer->mExcludeAreasAtTheEdges.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);
  connect(mParentContainer->mExcludeAreasWithoutCenterOfMass.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);

  //
  // Preprocessing
  //

  // auto [functionContainer, _7]      = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);

  // _7->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

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
    connect(this, &PanelVoronoiEdit::updatePreviewStarted, mSpinner, &WaitingSpinnerWidget::start);
    connect(this, &PanelVoronoiEdit::updatePreviewFinished, mSpinner, &WaitingSpinnerWidget::stop);

    imageSubTitle->addWidget(mSpinner);

    imageSubTitle->addStretch(1);

    preview->addWidget(imageSubTitleWidget);
    */

  setLayout(horizontalLayout);
  horizontalLayout->addStretch();

  //
  // Signals from extern
  //
  connect(mWindowMain->getFoundFilesCombo(), &QComboBox::currentIndexChanged, this, &PanelVoronoiEdit::updatePreview);
  connect(mWindowMain->getImageSeriesCombo(), &QComboBox::currentIndexChanged, this, &PanelVoronoiEdit::updatePreview);
}

PanelVoronoiEdit::~PanelVoronoiEdit()
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

void PanelVoronoiEdit::onCellApproximationChanged()
{
  updatePreview();
}

void PanelVoronoiEdit::onChannelTypeChanged()
{
  updatePreview();
}

void PanelVoronoiEdit::updatePreview()
{
}

}    // namespace joda::ui::qt
