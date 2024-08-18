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

///

#include "panel_pipeline_settings.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include "ui/container/container_base.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelPipelineSettings::PanelPipelineSettings(WindowMain *wm, joda::settings::Pipeline &settings) :
    QWidget(wm), mLayout(this, true), mWindowMain(wm), mSettings(settings)
{
  setObjectName("PanelPipelineSettings");

  {
    auto *col1 = mLayout.addVerticalPanel();
    col1->addGroup("Meta", {mChannelName, mColorAndChannelIndex});
  }

  {
    auto *col2 = mLayout.addVerticalPanel();
  }

  {
    auto *col4    = mLayout.addVerticalPanel();
    mPreviewImage = new PanelPreview(PREVIEW_BASE_SIZE, PREVIEW_BASE_SIZE, this);
    mPreviewImage->setContentsMargins(0, 0, 0, 0);
    mPreviewImage->resetImage("");
    col4->addWidget(mPreviewImage);
  }

  connect(this, &PanelPipelineSettings::updatePreviewStarted, this, &PanelPipelineSettings::onPreviewStarted);
  connect(this, &PanelPipelineSettings::updatePreviewFinished, this, &PanelPipelineSettings::onPreviewFinished);

  connect(mPreviewImage, &PanelPreview::tileClicked, this, &PanelPipelineSettings::onTileClicked);
  connect(wm->getImagePanel(), &PanelImages::imageSelectionChanged, this, &PanelPipelineSettings::updatePreview);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::createSettings(WindowMain *windowMain)
{
  mChannelName = std::shared_ptr<Setting<QString, QString>>(
      new Setting<QString, QString>("icons8-text-50.png", "Name", "Channel Name", "Name", windowMain));
  mChannelName->setMaxLength(15);

  mColorAndChannelIndex = std::shared_ptr<Setting<QString, int32_t>>(new Setting<QString, int32_t>(
      "icons8-unknown-status-50.png", "Type", "Channel index", "", "#B91717",
      {{"#B91717", "", "icons8-bubble-50red-#B91717.png"},
       {"#06880C", "", "icons8-bubble-50 -green-#06880C.png"},
       {"#1771B9", "", "icons8-bubble-blue-#1771B9-50.png"},
       {"#FBEA25", "", "icons8-bubble-50-yellow-#FBEA25.png"},
       {"#6F03A6", "", "icons8-bubble-50-violet-#6F03A6.png"},
       {"#818181", "", "icons8-bubble-50-gray-#818181.png"},
       /*{"#000000", "", "icons8-bubble-50-black-#000000.png"}*/},
      {{0, "Channel 0"}, {1, "Channel 1"}, {2, "Channel 2"}, {3, "Channel 3"}, {4, "Channel 4"}, {5, "Channel 5"}}, 0,
      windowMain));

  mOverview = new PanelChannelOverview(windowMain, this);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelPipelineSettings::~PanelPipelineSettings()
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::valueChangedEvent()
{
  updatePreview(-1, -1);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::updatePreview(int32_t /**/, int32_t /**/)
{
  auto [newImgIdex, selectedSeries] = mWindowMain->getImagePanel()->getSelectedImage();
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
      mPreviewThread = std::make_unique<std::thread>([this, newImgIdex = newImgIdex,
                                                      selectedSeries = selectedSeries]() {
        int previewCounter = 0;
        std::this_thread::sleep_for(500ms);
        do {
          if(nullptr != mPreviewImage) {
            std::filesystem::path imgIndex = newImgIdex;
            if(!imgIndex.empty()) {
              auto *controller = mWindowMain->getController();
              try {
                int32_t resolution = 0;
                uint32_t series    = selectedSeries;
                auto tileSize      = controller->getCompositeTileSize();
                auto imgProps      = controller->getImageProperties(imgIndex, series);
                auto [tileNrX, tileNrY] =
                    imgProps.getImageInfo().resolutions.at(resolution).getNrOfTiles(tileSize.width, tileSize.height);

                controller->preview(getPipeline(), imgIndex, mSelectedTileX, mSelectedTileY, mPreviewObject);
                auto &previewResult = mPreviewImage->getPreviewObject();
                // Create a QByteArray from the char array
                int valid   = 0;
                int invalid = 0;
                /* for(const auto &roi : *previewResult.detectionResult) {
                   if(roi.isValid()) {
                     valid++;
                   } else {
                     invalid++;
                   }
                 }*/

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
void PanelPipelineSettings::onTileClicked(int32_t tileX, int32_t tileY)
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
void PanelPipelineSettings::onPreviewStarted()
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
void PanelPipelineSettings::onPreviewFinished()
{
  if(nullptr != mPreviewImage) {
    mPreviewImage->setWaiting(false);
  }
}

}    // namespace joda::ui::qt
