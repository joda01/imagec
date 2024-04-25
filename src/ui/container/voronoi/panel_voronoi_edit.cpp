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
#include "../../window_main.hpp"
#include "../container_function.hpp"
#include "../dialog_channel_measurment.hpp"
#include "container_voronoi.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

PanelVoronoiEdit::PanelVoronoiEdit(WindowMain *wm, ContainerVoronoi *parentContainer) :
    mWindowMain(wm), mParentContainer(parentContainer)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelVoronoiEdit");
}

void PanelVoronoiEdit::init()
{
  auto *horizontalLayout = createLayout();

  auto [verticalLayoutContainer, _1] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto [verticalLayoutMeta, _2]      = addVerticalPanel(verticalLayoutContainer, "rgba(0, 104, 117, 0.05)");
  verticalLayoutMeta->addWidget(createTitle("Meta"));
  verticalLayoutMeta->addWidget(mParentContainer->mChannelName->getEditableWidget());
  verticalLayoutMeta->addWidget(mParentContainer->mColorAndChannelIndex->getEditableWidget());
  _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  connect(mParentContainer->mColorAndChannelIndex.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);

  //
  // Cross channel
  //
  auto [llayoutColoc, _11] = addVerticalPanel(verticalLayoutContainer, "rgba(0, 104, 117, 0.05)");
  llayoutColoc->addWidget(createTitle("Cross-Channel"));
  llayoutColoc->addWidget(mParentContainer->mVoronoiPoints->getEditableWidget());
  // llayoutColoc->addWidget(parentContainer->mColocGroup->getEditableWidget());
  llayoutColoc->addWidget(mParentContainer->mCrossChannelIntensity->getEditableWidget());
  llayoutColoc->addWidget(mParentContainer->mCrossChannelCount->getEditableWidget());
  _11->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  verticalLayoutContainer->addStretch(0);

  auto [detectionContainer, _4] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);

  auto [verticalLayoutFilter, _6] = addVerticalPanel(detectionContainer, "rgba(0, 104, 117, 0.05)", 16, false);
  verticalLayoutFilter->addWidget(createTitle("Filtering"));
  verticalLayoutFilter->addWidget(mParentContainer->mMaxVoronoiAreaSize->getEditableWidget());
  verticalLayoutFilter->addWidget(mParentContainer->mOverlayMaskChannelIndex->getEditableWidget());

  verticalLayoutFilter->addStretch();
  _6->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  connect(mParentContainer->mMaxVoronoiAreaSize.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);
  connect(mParentContainer->mOverlayMaskChannelIndex.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelVoronoiEdit::updatePreview);

  //
  // Preprocessing
  //

  auto [functionContainer, _7]      = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0);
  auto [verticalLayoutFuctions, _8] = addVerticalPanel(functionContainer, "rgba(0, 104, 117, 0.05)", 16, false);
  verticalLayoutFuctions->addWidget(createTitle("Measurement"));
  {
    QPushButton *editMeasurment = new QPushButton("Measured data");
    editMeasurment->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(0, 0, 0, 0);"
        "   border: 1px solid rgb(111, 121, 123);"
        "   color: rgb(0, 104, 117);"
        "   padding: 10px 20px;"
        "   border-radius: 4px;"
        "   font-size: 14px;"
        "   font-weight: normal;"
        "   text-align: center;"
        "   text-decoration: none;"
        "}"

        "QPushButton:hover {"
        "   background-color: rgba(0, 0, 0, 0);"    // Darken on hover
        "}"

        "QPushButton:pressed {"
        "   background-color: rgba(0, 0, 0, 0);"    // Darken on press
        "}");
    connect(editMeasurment, &QPushButton::pressed, this, &PanelVoronoiEdit::onEditMeasurementClicked);
    verticalLayoutFuctions->addWidget(editMeasurment);
  }

  _8->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  _7->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

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
  connect(mWindowMain->getImageTilesCombo(), &QComboBox::currentIndexChanged, this, &PanelVoronoiEdit::updatePreview);
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

QLabel *PanelVoronoiEdit::createTitle(const QString &title)
{
  auto *label = new QLabel();
  QFont font;
  font.setPixelSize(16);
  font.setBold(true);
  label->setFont(font);
  label->setText(title);

  return label;
}

QHBoxLayout *PanelVoronoiEdit::createLayout()
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

std::tuple<QVBoxLayout *, QWidget *> PanelVoronoiEdit::addVerticalPanel(QLayout *horizontalLayout,
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

///
/// \brief      Edit measurements for this channel
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelVoronoiEdit::onEditMeasurementClicked()
{
  DialogChannelMeasurement measure(this, mParentContainer->mSettings.reporting);
  measure.exec();
}

}    // namespace joda::ui::qt
