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
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <exception>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include "../../window_main.hpp"
#include "../container_function.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/settings/detection/detection_settings.hpp"
#include "container_channel.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

PanelChannelEdit::PanelChannelEdit(WindowMain *wm, ContainerChannel *parentContainer) :
    mWindowMain(wm), mParentContainer(parentContainer)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelChannelEdit");
  init();
}

void PanelChannelEdit::init()
{
  auto *horizontalLayout = createLayout();

  //
  // Column 1
  //
  // rgb(246, 246, 246)
  auto [verticalLayoutContainer, _1] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);
  auto [verticalLayoutMeta, _2]      = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
  verticalLayoutMeta->addWidget(createTitle("Meta"));
  verticalLayoutMeta->addWidget(mParentContainer->mChannelName->getEditableWidget());
  verticalLayoutMeta->addWidget(mParentContainer->mColorAndChannelIndex->getEditableWidget());
  verticalLayoutMeta->addWidget(mParentContainer->mChannelType->getEditableWidget());
  _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  connect(mParentContainer->mChannelType.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::onChannelTypeChanged);
  connect(mParentContainer->mColorAndChannelIndex.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);

  // Cross channel
  auto [llayoutColoc, _11] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
  llayoutColoc->addWidget(createTitle("Cross-Channel"));
  // llayoutColoc->addWidget(parentContainer->mColocGroup->getEditableWidget());
  llayoutColoc->addWidget(mParentContainer->mCrossChannelIntensity->getEditableWidget());
  llayoutColoc->addWidget(mParentContainer->mCrossChannelCount->getEditableWidget());
  _11->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  // Measurement
  // auto [measurement, _6] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)", 16, false);
  // measurement->addWidget(createTitle("Measurement"));

  verticalLayoutContainer->addStretch(0);

  //
  // Column 2
  //
  auto [functionContainer, _7]      = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);
  auto [verticalLayoutFuctions, _8] = addVerticalPanel(functionContainer, "rgb(246, 246, 246)", 16, false);
  verticalLayoutFuctions->addWidget(createTitle("Preprocessing"));
  verticalLayoutFuctions->addWidget(mParentContainer->mZProjection->getEditableWidget());
  verticalLayoutFuctions->addWidget(mParentContainer->mMarginCrop->getEditableWidget());
  verticalLayoutFuctions->addWidget(mParentContainer->mMedianBackgroundSubtraction->getEditableWidget());
  verticalLayoutFuctions->addWidget(mParentContainer->mEdgeDetection->getEditableWidget());
  verticalLayoutFuctions->addWidget(mParentContainer->mRollingBall->getEditableWidget());
  verticalLayoutFuctions->addWidget(mParentContainer->mSubtractChannel->getEditableWidget());
  verticalLayoutFuctions->addWidget(mParentContainer->mSmoothing->getEditableWidget());
  verticalLayoutFuctions->addWidget(mParentContainer->mGaussianBlur->getEditableWidget());
  _8->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  _7->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

  connect(mParentContainer->mZProjection.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mMarginCrop.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mMedianBackgroundSubtraction.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mEdgeDetection.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mRollingBall.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mSubtractChannel.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mSmoothing.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mGaussianBlur.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);

  //
  // Column 3
  //
  auto [detectionContainer, _4] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);
  auto [detection, _5]          = addVerticalPanel(detectionContainer, "rgb(246, 246, 246)");
  detection->addWidget(createTitle("Detection"));
  detection->addWidget(mParentContainer->mUsedDetectionMode->getEditableWidget());
  connect(mParentContainer->mUsedDetectionMode.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::onDetectionModechanged);
  detection->addWidget(mParentContainer->mThresholdAlgorithm->getEditableWidget());
  detection->addWidget(mParentContainer->mThresholdValueMin->getEditableWidget());
  detection->addWidget(mParentContainer->mAIModels->getEditableWidget());
  detection->addWidget(mParentContainer->mMinProbability->getEditableWidget());
  detection->addWidget(mParentContainer->mWateredSegmentation->getEditableWidget());
  detection->addWidget(mParentContainer->mSnapAreaSize->getEditableWidget());

  _5->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  onDetectionModechanged();

  connect(mParentContainer->mThresholdAlgorithm.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mThresholdValueMin.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mAIModels.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mMinProbability.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mWateredSegmentation.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mSnapAreaSize.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);

  //
  // Column 4
  //
  auto [filterContainer, filterContainerLayout] =
      addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);
  auto [objectFilter, objectFilterLayout] = addVerticalPanel(filterContainer, "rgb(246, 246, 246)");
  objectFilter->addWidget(createTitle("Object filter"));
  objectFilter->addWidget(mParentContainer->mMinParticleSize->getEditableWidget());
  objectFilter->addWidget(mParentContainer->mMaxParticleSize->getEditableWidget());
  objectFilter->addWidget(mParentContainer->mMinCircularity->getEditableWidget());
  objectFilter->addWidget(mParentContainer->mTetraspeckRemoval->getEditableWidget());
  objectFilter->addStretch();
  objectFilterLayout->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  connect(mParentContainer->mMinParticleSize.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mMaxParticleSize.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mMinCircularity.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);

  connect(mParentContainer->mTetraspeckRemoval.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);

  auto [imageFilter, imageFilterLayout] = addVerticalPanel(filterContainer, "rgb(246, 246, 246)", 16, false);
  imageFilter->addWidget(createTitle("Image filter"));
  imageFilter->addWidget(mParentContainer->mImageFilterMode->getEditableWidget());
  imageFilter->addWidget(mParentContainer->mMaxObjects->getEditableWidget());
  imageFilter->addWidget(mParentContainer->mHistogramThresholdFactor->getEditableWidget());

  connect(mParentContainer->mImageFilterMode.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mMaxObjects.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);
  connect(mParentContainer->mHistogramThresholdFactor.get(), &ContainerFunctionBase::valueChanged, this,
          &PanelChannelEdit::updatePreview);

  imageFilterLayout->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  //
  // Preview
  //
  auto [preview, _9] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, PREVIEW_BASE_SIZE, 16);
  mPreviewImage      = new PanelPreview(PREVIEW_BASE_SIZE, PREVIEW_BASE_SIZE, this);
  mPreviewImage->resetImage("");
  preview->addWidget(mPreviewImage);
  QWidget *imageSubTitleWidget = new QWidget();
  imageSubTitleWidget->setMinimumHeight(50);
  QHBoxLayout *imageSubTitle = new QHBoxLayout();
  imageSubTitleWidget->setLayout(imageSubTitle);

  //
  // Signals from extern
  //
  connect(this, &PanelChannelEdit::updatePreviewStarted, this, &PanelChannelEdit::onPreviewStarted);
  connect(this, &PanelChannelEdit::updatePreviewFinished, this, &PanelChannelEdit::onPreviewFinished);

  imageSubTitle->addStretch(1);

  preview->addWidget(imageSubTitleWidget);

  setLayout(horizontalLayout);
  horizontalLayout->addStretch();

  //
  // Signals from extern
  //
  connect(mPreviewImage, &PanelPreview::tileClicked, this, &PanelChannelEdit::onTileClicked);
  connect(mWindowMain->getFoundFilesCombo(), &QComboBox::currentIndexChanged, this, &PanelChannelEdit::updatePreview);
  connect(mWindowMain->getImageSeriesCombo(), &QComboBox::currentIndexChanged, this, &PanelChannelEdit::updatePreview);
  connect(mWindowMain->getImageResolutionCombo(), &QComboBox::currentIndexChanged, this,
          &PanelChannelEdit::updatePreview);
}

PanelChannelEdit::~PanelChannelEdit()
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
                                                                        bool enableScrolling, int maxWidth,
                                                                        int spacing) const
{
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(spacing);
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

void PanelChannelEdit::onChannelTypeChanged()
{
  updatePreview();
}

void PanelChannelEdit::onDetectionModechanged()
{
  if(mParentContainer->mUsedDetectionMode->getValue() == joda::settings::DetectionSettings::DetectionMode::AI) {
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

  updatePreview();
}

void PanelChannelEdit::updatePreview()
{
  if(mIsActiveShown) {
    if(mPreviewCounter == 0) {
      {
        std::lock_guard<std::mutex> lock(mPreviewMutex);
        mPreviewCounter++;
        emit updatePreviewStarted();
      }
      if(mPreviewThread != nullptr) {
        if(mPreviewThread->joinable()) {
          mPreviewThread->join();
        }
      }
      mPreviewThread = std::make_unique<std::thread>([this]() {
        int previewCounter = 0;
        std::this_thread::sleep_for(500ms);
        do {
          if(nullptr != mPreviewImage) {
            int imgIndex = mWindowMain->getSelectedFileIndex();
            if(imgIndex >= 0) {
              auto *controller = mWindowMain->getController();
              try {
                int32_t resolution = mWindowMain->getImageResolutionCombo()->currentData().toInt();
                int32_t series     = mWindowMain->getImageSeriesCombo()->currentData().toInt();
                mParentContainer->toSettings();
                auto imgProps = mWindowMain->getController()->getImageProperties(imgIndex, series);
                auto [tileNrX, tileNrY] =
                    imgProps.getImageInfo()
                        .resolutions.at(resolution)
                        .getNrOfTiles(::joda::pipeline::COMPOSITE_TILE_WIDTH, ::joda::pipeline::COMPOSITE_TILE_HEIGHT);

                controller->preview(mParentContainer->mSettings, imgIndex, mSelectedTileX, mSelectedTileY, resolution,
                                    mPreviewImage->getPreviewObject());
                auto &previewResult = mPreviewImage->getPreviewObject();
                // Create a QByteArray from the char array
                int valid   = 0;
                int invalid = 0;
                for(const auto &roi : *previewResult.detectionResult) {
                  if(roi.isValid()) {
                    valid++;
                  } else {
                    invalid++;
                  }
                }

                mPreviewImage->setThumbnailPosition(tileNrX, tileNrY, mSelectedTileX, mSelectedTileY);

                QString info("Valid: " + QString::number(valid) + " | Invalid: " + QString::number(invalid));
                mPreviewImage->updateImage(info);
                if(!mIsActiveShown) {
                  mPreviewImage->resetImage("");
                }

              } catch(const std::exception &error) {
                // mPreviewImage->resetImage(error.what());
              }
            }
          }
          std::this_thread::sleep_for(250ms);
          {
            std::lock_guard<std::mutex> lock(mPreviewMutex);
            previewCounter = mPreviewCounter;
            previewCounter--;
            mPreviewCounter = previewCounter;
          }
        } while(previewCounter > 0);
        emit updatePreviewFinished();
      });
    } else {
      std::lock_guard<std::mutex> lock(mPreviewMutex);
      mPreviewCounter++;
    }
  } else {
    if(nullptr != mPreviewImage) {
      mPreviewImage->resetImage("");
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelChannelEdit::onTileClicked(int32_t tileX, int32_t tileY)
{
  mSelectedTileX = tileX;
  mSelectedTileY = tileY;
  updatePreview();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelChannelEdit::onPreviewStarted()
{
  if(nullptr != mPreviewImage) {
    mPreviewImage->setWaiting(true);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelChannelEdit::onPreviewFinished()
{
  if(nullptr != mPreviewImage) {
    mPreviewImage->setWaiting(false);
  }
}

}    // namespace joda::ui::qt
