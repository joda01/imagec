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
#include "backend/commands/image_functions/median_substraction/median_substraction_settings.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/container/command/factory.hpp"
#include "ui/container/container_base.hpp"
#include "ui/window_main/window_main.hpp"
#include "add_command_button.hpp"

namespace joda::ui {

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
  createSettings(wm);

  {
    auto *col1 = mLayout.addVerticalPanel();
    col1->addGroup("Pipeline setup", {mPipelineName, mDefaultClusterId, mCStackIndex, mZProjection});
  }

  {
    auto *col2 = mLayout.addVerticalPanel();

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setFrameStyle(0);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->viewport()->setStyleSheet("background-color: transparent;");
    scrollArea->setObjectName("scrollAreaOverview");
    // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create a widget to hold the panels
    auto *contentWidget = new QWidget(scrollArea);
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentWidget->setObjectName("contentOverview");
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    // Create a horizontal layout for the pipeline steps
    mPipelineSteps = new QVBoxLayout(contentWidget);
    mPipelineSteps->setContentsMargins(0, 0, 0, 0);
    mPipelineSteps->setSpacing(8);    // Adjust this value as needed
    mPipelineSteps->setAlignment(Qt::AlignTop);
    contentWidget->setLayout(mPipelineSteps);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    col2->addGroup("Pipeline steps", {scrollArea});
  }

  {
    auto *col3 = mLayout.addVerticalPanel();
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
void PanelPipelineSettings::addPipelineStep(std::shared_ptr<joda::ui::Command> command)
{
  mCommands.push_back(command);
  connect(command.get(), &joda::ui::Command::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  mPipelineSteps->addWidget(command.get());
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
  mPipelineName = std::shared_ptr<Setting<std::string, std::string>>(
      new Setting<std::string, std::string>("icons8-text-50.png", "Name", "Pipeline name", "Name", windowMain));
  mPipelineName->setMaxLength(15);
  mPipelineName->connectWithSetting(&mSettings.meta.name, nullptr);

  mCStackIndex = std::shared_ptr<Setting<std::string, int32_t>>(
      new Setting<std::string, int32_t>("icons8-unknown-status-50.png", "Type", "Image channel", "", "#B91717",
                                        {{"#B91717", "", "icons8-bubble-50red-#B91717.png"},
                                         {"#06880C", "", "icons8-bubble-50 -green-#06880C.png"},
                                         {"#1771B9", "", "icons8-bubble-blue-#1771B9-50.png"},
                                         {"#FBEA25", "", "icons8-bubble-50-yellow-#FBEA25.png"},
                                         {"#6F03A6", "", "icons8-bubble-50-violet-#6F03A6.png"},
                                         {"#818181", "", "icons8-bubble-50-gray-#818181.png"},
                                         /*{"#000000", "", "icons8-bubble-50-black-#000000.png"}*/},
                                        {{0, "Channel 0"},
                                         {1, "Channel 1"},
                                         {2, "Channel 2"},
                                         {3, "Channel 3"},
                                         {4, "Channel 4"},
                                         {5, "Channel 5"},
                                         {6, "Channel 6"},
                                         {7, "Channel 7"},
                                         {8, "Channel 8"},
                                         {9, "Channel 9"},
                                         {10, "Channel 10"}},
                                        0, windowMain));
  mCStackIndex->connectWithSetting(&mSettings.meta.color, &mSettings.pipelineSetup.cStackIndex);

  mZProjection = std::shared_ptr<Setting<enums::ZProjection, int32_t>>(new Setting<enums::ZProjection, int32_t>(
      "icons8-layers-50.png", "Z-Projection", "Z-Projection", "", enums::ZProjection::NONE,
      {{enums::ZProjection::NONE, "Off"},
       {enums::ZProjection::MAX_INTENSITY, "Max. intensity"},
       {enums::ZProjection::MIN_INTENSITY, "Min. intensity"},
       {enums::ZProjection::AVG_INTENSITY, "Avg'. intensity"}},
      windowMain, "z_projection.json"));
  mZProjection->connectWithSetting(&mSettings.pipelineSetup.zProjection, nullptr);

  mDefaultClusterId = std::shared_ptr<Setting<enums::ClusterIdIn, int32_t>>(new Setting<enums::ClusterIdIn, int32_t>(
      "icons8-connection-50.png", "Cluster", "Cluster", "", enums::ClusterIdIn::A,
      {
          {enums::ClusterIdIn::A, "Cluster A"},
          {enums::ClusterIdIn::B, "Cluster B"},
          {enums::ClusterIdIn::C, "Cluster C"},
          {enums::ClusterIdIn::D, "Cluster D"},
          {enums::ClusterIdIn::E, "Cluster E"},
          {enums::ClusterIdIn::F, "Cluster F"},
          {enums::ClusterIdIn::G, "Cluster G"},
          {enums::ClusterIdIn::H, "Cluster H"},
          {enums::ClusterIdIn::I, "Cluster I"},
          {enums::ClusterIdIn::J, "Cluster J"},
      },
      windowMain, "z_projection.json"));
  mDefaultClusterId->connectWithSetting(&mSettings.pipelineSetup.defaultClusterId, nullptr);

  connect(mPipelineName.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::metaChangedEvent);
  connect(mCStackIndex.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(mZProjection.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(mDefaultClusterId.get(), &joda::ui::SettingBase::valueChanged, this,
          &PanelPipelineSettings::valueChangedEvent);

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
void PanelPipelineSettings::metaChangedEvent()
{
  toSettings();
  mWindowMain->checkForSettingsChanged();
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
          toSettings();
          mWindowMain->checkForSettingsChanged();
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::fromSettings(const joda::settings::Pipeline &settings)
{
  mSettings.meta          = settings.meta;
  mSettings.pipelineSetup = settings.pipelineSetup;

  mPipelineName->setValue(mSettings.meta.name);
  mCStackIndex->setValueSecond(mSettings.pipelineSetup.cStackIndex);
  mZProjection->setValue(mSettings.pipelineSetup.zProjection);
  mDefaultClusterId->setValue(mSettings.pipelineSetup.defaultClusterId);

  //
  // Pipelinesteps
  //
  mSettings.pipelineSteps.clear();
  for(auto &step : settings.pipelineSteps) {
    mSettings.pipelineSteps.push_back(step);
    auto &cmdSetting = mSettings.pipelineSteps.back();

    auto cmd = joda::settings::PipelineFactory<joda::ui::Command>::generate(cmdSetting, mWindowMain);
    if(cmd != nullptr) {
      addPipelineStep(cmd);
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
void PanelPipelineSettings::toSettings()
{
  mSettings.meta.name                      = mPipelineName->getValue();
  mSettings.pipelineSetup.cStackIndex      = mCStackIndex->getValueSecond();
  mSettings.pipelineSetup.zProjection      = mZProjection->getValue();
  mSettings.pipelineSetup.defaultClusterId = mDefaultClusterId->getValue();
}

}    // namespace joda::ui