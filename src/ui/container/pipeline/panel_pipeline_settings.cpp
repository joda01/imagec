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
#include <qaction.h>
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
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/processor/processor.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "backend/settings/project_settings/project_cluster.hpp"
#include "ui/container/command/factory.hpp"
#include "ui/container/container_base.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/template_parser/template_parser.hpp"
#include "ui/window_main/panel_classification.hpp"
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
  auto *tab = mLayout.addTab("", [] {});
  createSettings(tab, wm);

  {
    auto *col2 = tab->addVerticalPanel();

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setFrameStyle(0);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->viewport()->setStyleSheet("background-color: transparent;");
    scrollArea->setObjectName("scrollAreaOverview");
    // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create a widget to hold the panels
    auto *contentWidget = new QWidget(scrollArea);
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentWidget->setContentsMargins(0, 0, 0, 0);
    contentWidget->setObjectName("contentOverview");
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    // Create a horizontal layout for the pipeline steps
    mPipelineSteps = new QVBoxLayout(contentWidget);
    mPipelineSteps->setContentsMargins(0, 0, 0, 0);
    mPipelineSteps->setSpacing(4);    // Adjust this value as needed
    mPipelineSteps->setAlignment(Qt::AlignTop);
    contentWidget->setLayout(mPipelineSteps);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    col2->addWidgetGroup("Pipeline steps", {scrollArea});

    mPipelineSteps->addWidget(new AddCommandButtonBase(mSettings, this, nullptr, mWindowMain));
  }

  {
    auto *col3 = tab->addVerticalPanel();
  }

  {
    auto *col4    = tab->addVerticalPanel();
    mPreviewImage = new PanelPreview(PREVIEW_BASE_SIZE, PREVIEW_BASE_SIZE, this);
    mPreviewImage->setContentsMargins(0, 0, 0, 0);
    mPreviewImage->resetImage("");
    col4->addWidget(mPreviewImage);
  }

  // Tool button

  auto *saveAsTemplateButton = mLayout.addActionButton("Save as template", "icons8-mark-as-favorite-50.png");
  connect(saveAsTemplateButton, &QAction::triggered, [this] { this->saveAsTemplate(); });

  connect(this, &PanelPipelineSettings::updatePreviewStarted, this, &PanelPipelineSettings::onPreviewStarted);
  connect(this, &PanelPipelineSettings::updatePreviewFinished, this, &PanelPipelineSettings::onPreviewFinished);
  connect(mPreviewImage, &PanelPreview::tileClicked, this, &PanelPipelineSettings::onTileClicked);
  connect(mPreviewImage, &PanelPreview::onSettingChanged, this, &PanelPipelineSettings::updatePreview);
  connect(wm->getImagePanel(), &PanelImages::imageSelectionChanged, this, &PanelPipelineSettings::updatePreview);
  connect(mLayout.getBackButton(), &QAction::triggered, this, &PanelPipelineSettings::closeWindow);
  connect(mLayout.getDeleteButton(), &QAction::triggered, this, &PanelPipelineSettings::deletePipeline);
  connect(wm->getPanelClassification(), &PanelClassification::settingsChanged, this,
          &PanelPipelineSettings::onClassificationNameChanged);
  onClassificationNameChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::addPipelineStep(std::unique_ptr<joda::ui::Command> command,
                                            const settings::PipelineStep *pipelineStepBefore)
{
  command->registerDeleteButton(this);
  connect(command.get(), &joda::ui::Command::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  mPipelineSteps->addWidget(command.get());
  mPipelineSteps->addWidget(new AddCommandButtonBase(mSettings, this, pipelineStepBefore, mWindowMain));
  mCommands.push_back(std::move(command));
  mWindowMain->checkForSettingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::insertNewPipelineStep(int32_t posToInsert, std::unique_ptr<joda::ui::Command> command,
                                                  const settings::PipelineStep *pipelineStepBefore)
{
  command->registerDeleteButton(this);
  connect(command.get(), &joda::ui::Command::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  int widgetPos = (posToInsert * 2) + 1;    // Each second is a button
  mPipelineSteps->insertWidget(widgetPos, command.get());
  mPipelineSteps->insertWidget(widgetPos + 1,
                               new AddCommandButtonBase(mSettings, this, pipelineStepBefore, mWindowMain));
  mCommands.insert(mCommands.begin() + posToInsert, std::move(command));
  mWindowMain->checkForSettingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::erasePipelineStep(const Command *toDelete)
{
  for(int index = 0; index < mPipelineSteps->count(); index++) {
    if(toDelete == mPipelineSteps->itemAt(index)->widget()) {
      // Delete add button
      {
        QWidget *widget = mPipelineSteps->itemAt(index + 1)->widget();
        mPipelineSteps->removeWidget(widget);
        widget->setParent(nullptr);
      }

      // Delete command widget
      {
        QWidget *widget = mPipelineSteps->itemAt(index)->widget();
        mPipelineSteps->removeWidget(widget);
        widget->setParent(nullptr);
      }

      // Delete settings
      {
        int toDeleteIndex = index / 2;    // Each second is a add button
        const auto &it    = std::next(mSettings.pipelineSteps.begin(), toDeleteIndex);
        mSettings.pipelineSteps.erase(it);
      }

      // Delete command from vector
      {
        for(auto it = mCommands.begin(); it != mCommands.end(); it++) {
          if(it->get() == toDelete) {
            mCommands.erase(it);
            break;
          }
        }
      }

      return;
    }
  }
  mWindowMain->checkForSettingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::createSettings(helper::TabWidget *tab, WindowMain *windowMain)
{
  //
  //
  pipelineName = SettingBase::create<SettingLineEdit<std::string>>(windowMain, "icons8-text-50.png", "Pipeline name");
  pipelineName->setPlaceholderText("Name");
  pipelineName->setMaxLength(15);
  pipelineName->connectWithSetting(&mSettings.meta.name);

  //
  //
  cStackIndex =
      SettingBase::create<SettingComboBox<int32_t>>(windowMain, "icons8-unknown-status-50.png", "Image channel");
  cStackIndex->addOptions({{-1, "Empty"},
                           {0, "Channel 0"},
                           {1, "Channel 1"},
                           {2, "Channel 2"},
                           {3, "Channel 3"},
                           {4, "Channel 4"},
                           {5, "Channel 5"},
                           {6, "Channel 6"},
                           {7, "Channel 7"},
                           {8, "Channel 8"},
                           {9, "Channel 9"},
                           {10, "Channel 10"}});
  cStackIndex->setDefaultValue(-1);
  cStackIndex->connectWithSetting(&mSettings.pipelineSetup.cStackIndex);

  //
  //
  zProjection =
      SettingBase::create<SettingComboBox<enums::ZProjection>>(windowMain, "icons8-layers-50.png", "Z-Projection");
  zProjection->addOptions({{enums::ZProjection::NONE, "Off"},
                           {enums::ZProjection::MAX_INTENSITY, "Max. intensity"},
                           {enums::ZProjection::MIN_INTENSITY, "Min. intensity"},
                           {enums::ZProjection::AVG_INTENSITY, "Avg'. intensity"}});
  zProjection->connectWithSetting(&mSettings.pipelineSetup.zProjection);

  //
  //
  defaultClusterId =
      SettingBase::create<SettingComboBox<enums::ClusterId>>(windowMain, "icons8-connection-50.png", "Cluster");
  defaultClusterId->addOptions({{enums::ClusterId::A, "Cluster A"},
                                {enums::ClusterId::B, "Cluster B"},
                                {enums::ClusterId::C, "Cluster C"},
                                {enums::ClusterId::D, "Cluster D"},
                                {enums::ClusterId::E, "Cluster E"},
                                {enums::ClusterId::F, "Cluster F"},
                                {enums::ClusterId::G, "Cluster G"},
                                {enums::ClusterId::H, "Cluster H"},
                                {enums::ClusterId::I, "Cluster I"},
                                {enums::ClusterId::J, "Cluster J"}});
  defaultClusterId->connectWithSetting(&mSettings.pipelineSetup.defaultClusterId);

  connect(pipelineName.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::metaChangedEvent);
  connect(cStackIndex.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(zProjection.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(defaultClusterId.get(), &joda::ui::SettingBase::valueChanged, this,
          &PanelPipelineSettings::valueChangedEvent);

  {
    auto *col1 = tab->addVerticalPanel();
    col1->addGroup("Pipeline setup",
                   {pipelineName.get(), cStackIndex.get(), zProjection.get(), defaultClusterId.get()});
  }

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
  // if(cStackIndex < 0) {
  //   zProjection->getDisplayLabelWidget()->setVisible(false);
  // } else {
  //   zProjection->getDisplayLabelWidget()->setVisible(true);
  // }
  updatePreview();
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
void PanelPipelineSettings::updatePreview()
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

                auto &previewResult = mPreviewImage->getPreviewObject();
                processor::PreviewSettings prevSettings;
                prevSettings.style = mPreviewImage->getFilledPreview() ? settings::ImageSaverSettings::Style::FILLED
                                                                       : settings::ImageSaverSettings::Style::OUTLINED;
                controller->preview(mWindowMain->getSettings().imageSetup, prevSettings, mWindowMain->getSettings(),
                                    getPipeline(), imgIndex, mSelectedTileX, mSelectedTileY, previewResult);
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
                joda::log::logError("Preview error: " + std::string(error.what()));
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
      if(mPreviewCounter > 1) {
        mPreviewCounter = 1;
      }
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
  updatePreview();
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

  pipelineName->setValue(settings.meta.name);
  cStackIndex->setValue(settings.pipelineSetup.cStackIndex);
  zProjection->setValue(settings.pipelineSetup.zProjection);
  defaultClusterId->setValue(settings.pipelineSetup.defaultClusterId);

  //
  // Pipelinesteps
  //
  mSettings.pipelineSteps.clear();
  for(const joda::settings::PipelineStep &step : settings.pipelineSteps) {
    mSettings.pipelineSteps.emplace_back(step);
    auto &cmdSetting = mSettings.pipelineSteps.back();
    std::unique_ptr<joda::ui::Command> cmd =
        joda::settings::PipelineFactory<joda::ui::Command>::generate(cmdSetting, mWindowMain);
    if(cmd != nullptr) {
      addPipelineStep(std::move(cmd), &cmdSetting);
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
  mSettings.meta.name                      = pipelineName->getValue();
  mSettings.pipelineSetup.cStackIndex      = cStackIndex->getValue();
  mSettings.pipelineSetup.zProjection      = zProjection->getValue();
  mSettings.pipelineSetup.defaultClusterId = defaultClusterId->getValue();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::closeWindow()
{
  mWindowMain->showPanelStartPage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::deletePipeline()
{
  QMessageBox messageBox(mWindowMain);
  auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
  messageBox.setIconPixmap(icon->pixmap(42, 42));
  messageBox.setWindowTitle("Delete pipeline?");
  messageBox.setText("Delete pipeline?");
  QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  messageBox.setDefaultButton(noButton);
  auto reply = messageBox.exec();
  if(messageBox.clickedButton() == noButton) {
    return;
  }

  mWindowMain->showPanelStartPage();
  mWindowMain->getPanelPipeline()->erase(this);
}

///
/// \brief        Names in the classification has been changed, update all commands using
///               Class or Cluster with the new name
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::onClassificationNameChanged()
{
  const auto [clusters, classes] = mWindowMain->getPanelClassification()->getClustersAndClasses();
  std::map<enums::ClusterId, QString> clustersN;
  for(const auto &[id, name] : clusters) {
    if(id != enums::ClusterIdIn::$) {
      clustersN.emplace(static_cast<enums::ClusterId>(id), name);
    }
  }
  defaultClusterId->changeOptionText(clustersN);
}

///
/// \brief      Save as template
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::saveAsTemplate()
{
  QString folderToOpen           = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
  QString filePathOfSettingsFile = QFileDialog::getSaveFileName(
      this, "Save template", folderToOpen,
      "ImageC template files (*" + QString(joda::templates::TemplateParser::TEMPLATE_ENDIAN.data()) + ")");
  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }

  try {
    nlohmann::json templateJson = mSettings;
    joda::templates::TemplateParser::saveTemplate(templateJson,
                                                  std::filesystem::path(filePathOfSettingsFile.toStdString()));
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(mWindowMain);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Could not save template!");
    messageBox.setText("Could not save template, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

}    // namespace joda::ui
