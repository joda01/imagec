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
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <exception>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include "../container_function.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/settings/detection/detection_settings.hpp"
#include "ui/container/panel_edit_base.hpp"
#include "ui/window_main/window_main.hpp"
#include "container_channel.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

PanelChannelEdit::PanelChannelEdit(WindowMain *wm, ContainerChannel *parentContainer) :
    PanelEdit(wm, parentContainer), mParentContainer(parentContainer)
{
  setObjectName("PanelChannelEdit");

  {
    auto *col1 = layout().addVerticalPanel();
    col1->addGroup("Meta", {mParentContainer->mChannelName, mParentContainer->mColorAndChannelIndex,
                            mParentContainer->mChannelType});

    col1->addGroup("Cross channel", {mParentContainer->mCrossChannelIntensity, mParentContainer->mCrossChannelCount});

    col1->addGroup("Image filter", {mParentContainer->mImageFilterMode, mParentContainer->mMaxObjects,
                                    mParentContainer->mHistogramThresholdFactor});
  }

  {
    auto *col2 = layout().addVerticalPanel();
    col2->addGroup("Preprocessing", {mParentContainer->mZProjection, mParentContainer->mMarginCrop,
                                     mParentContainer->mMedianBackgroundSubtraction, mParentContainer->mEdgeDetection,
                                     mParentContainer->mRollingBall, mParentContainer->mSubtractChannel,
                                     mParentContainer->mSmoothing, mParentContainer->mGaussianBlur});
  }

  {
    auto *col3 = layout().addVerticalPanel();
    col3->addGroup("Detection", {mParentContainer->mUsedDetectionMode, mParentContainer->mThresholdAlgorithm,
                                 mParentContainer->mThresholdValueMin, mParentContainer->mThresholdValueMax,
                                 mParentContainer->mAIModels, mParentContainer->mMinProbability,
                                 mParentContainer->mWateredSegmentation, mParentContainer->mSnapAreaSize});
    col3->addGroup("Filter", {mParentContainer->mMinParticleSize, mParentContainer->mMaxParticleSize,
                              mParentContainer->mMinCircularity, mParentContainer->mTetraspeckRemoval});
  }

  {
    auto *col4    = layout().addVerticalPanel();
    mPreviewImage = new PanelPreview(PREVIEW_BASE_SIZE, PREVIEW_BASE_SIZE, this);
    mPreviewImage->setContentsMargins(0, 0, 0, 0);
    mPreviewImage->resetImage("");
    col4->addWidget(mPreviewImage);
  }

  connect(this, &PanelChannelEdit::updatePreviewStarted, this, &PanelChannelEdit::onPreviewStarted);
  connect(this, &PanelChannelEdit::updatePreviewFinished, this, &PanelChannelEdit::onPreviewFinished);

  connect(mPreviewImage, &PanelPreview::tileClicked, this, &PanelChannelEdit::onTileClicked);
  connect(getWindowMain()->getImagePanel(), &PanelImages::imageSelectionChanged, this,
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

void PanelChannelEdit::valueChangedEvent()
{
  if(mParentContainer->mUsedDetectionMode->getValue() == joda::settings::DetectionSettings::DetectionMode::AI) {
    mParentContainer->mMinProbability->getEditableWidget()->setVisible(true);
    mParentContainer->mAIModels->getEditableWidget()->setVisible(true);

    mParentContainer->mThresholdAlgorithm->getEditableWidget()->setVisible(false);
    mParentContainer->mThresholdValueMin->getEditableWidget()->setVisible(false);
    mParentContainer->mThresholdValueMax->getEditableWidget()->setVisible(false);

  } else {
    mParentContainer->mMinProbability->getEditableWidget()->setVisible(false);
    mParentContainer->mAIModels->getEditableWidget()->setVisible(false);

    mParentContainer->mThresholdAlgorithm->getEditableWidget()->setVisible(true);
    mParentContainer->mThresholdValueMin->getEditableWidget()->setVisible(true);
    mParentContainer->mThresholdValueMax->getEditableWidget()->setVisible(true);
  }

  updatePreview(-1, -1);
}

void PanelChannelEdit::updatePreview(int32_t /**/, int32_t /**/)
{
  auto [newImgIdex, selectedSeries] = getWindowMain()->getImagePanel()->getSelectedImage();
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
      mPreviewThread =
          std::make_unique<std::thread>([this, newImgIdex = newImgIdex, selectedSeries = selectedSeries]() {
            int previewCounter = 0;
            std::this_thread::sleep_for(500ms);
            do {
              if(nullptr != mPreviewImage) {
                int imgIndex = newImgIdex;
                if(imgIndex >= 0) {
                  auto *controller = getWindowMain()->getController();
                  try {
                    int32_t resolution = 0;
                    int32_t series     = selectedSeries;
                    mParentContainer->toSettings();
                    auto [imgProps, _]      = getWindowMain()->getController()->getImageProperties(imgIndex, series);
                    auto [tileNrX, tileNrY] = imgProps.getImageInfo()
                                                  .resolutions.at(resolution)
                                                  .getNrOfTiles(::joda::pipeline::COMPOSITE_TILE_WIDTH,
                                                                ::joda::pipeline::COMPOSITE_TILE_HEIGHT);

                    controller->preview(mParentContainer->mSettings, imgIndex, mSelectedTileX, mSelectedTileY,
                                        resolution, mPreviewImage->getPreviewObject());
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
  updatePreview(-1, -1);
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
