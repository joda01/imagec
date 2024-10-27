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
#include "ui/container/command/command.hpp"
#include "ui/container/command/factory.hpp"
#include "ui/container/container_base.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/template_parser/template_parser.hpp"
#include "ui/window_main/panel_classification.hpp"
#include "ui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>
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
    mPipelineSteps->setSpacing(0);    // Adjust this value as needed
    mPipelineSteps->setAlignment(Qt::AlignTop);
    contentWidget->setLayout(mPipelineSteps);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    col2->addWidgetGroup("Pipeline steps", {scrollArea}, 300, 300);

    // Allow to start with
    mTopAddCommandButton = new AddCommandButtonBase(mSettings, this, nullptr, InOuts::OBJECT, mWindowMain);
    mPipelineSteps->addWidget(mTopAddCommandButton);
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

  auto *saveAsTemplateButton = mLayout.addActionButton("Save as template", generateIcon("bookmark"));
  connect(saveAsTemplateButton, &QAction::triggered, [this] { this->saveAsTemplate(); });

  auto *copyPipeline = mLayout.addActionButton("Copy pipeline", generateIcon("copy"));
  connect(copyPipeline, &QAction::triggered, [this] { this->copyPipeline(); });

  connect(this, &PanelPipelineSettings::updatePreviewStarted, this, &PanelPipelineSettings::onPreviewStarted);
  connect(this, &PanelPipelineSettings::updatePreviewFinished, this, &PanelPipelineSettings::onPreviewFinished);
  connect(mPreviewImage, &PanelPreview::tileClicked, this, &PanelPipelineSettings::onTileClicked);
  connect(mPreviewImage, &PanelPreview::onSettingChanged, this, &PanelPipelineSettings::updatePreview);
  connect(wm->getImagePanel(), &PanelImages::imageSelectionChanged, this, &PanelPipelineSettings::updatePreview);
  connect(mLayout.getBackButton(), &QAction::triggered, this, &PanelPipelineSettings::closeWindow);
  connect(mLayout.getDeleteButton(), &QAction::triggered, this, &PanelPipelineSettings::deletePipeline);
  connect(wm->getPanelClassification(), &PanelClassification::settingsChanged, this, &PanelPipelineSettings::onClassificationNameChanged);
  onClassificationNameChanged();

  mPreviewThread = std::make_unique<std::thread>(&PanelPipelineSettings::previewThread, this);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::addPipelineStep(std::unique_ptr<joda::ui::Command> command, const settings::PipelineStep *pipelineStepBefore)
{
  command->blockComponentSignals(true);
  command->registerDeleteButton(this);
  command->registerAddCommandButton(mSettings, this, mWindowMain);
  connect(command.get(), &joda::ui::Command::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  mPipelineSteps->addWidget(command.get());
  command->blockComponentSignals(false);
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
  command->registerAddCommandButton(mSettings, this, mWindowMain);
  connect(command.get(), &joda::ui::Command::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  int widgetPos = posToInsert + 1;    // Each second is a button
  mPipelineSteps->insertWidget(widgetPos, command.get());
  mCommands.insert(mCommands.begin() + posToInsert, std::move(command));
  mWindowMain->checkForSettingsChanged();
  updatePreview();
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
      // Delete command widget
      {
        QWidget *widget = mPipelineSteps->itemAt(index)->widget();
        mPipelineSteps->removeWidget(widget);
        widget->setParent(nullptr);
      }

      // Delete settings
      {
        // First is not a command
        const auto &it = std::next(mSettings.pipelineSteps.begin(), (index - 1));
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
      updatePreview();
      mWindowMain->checkForSettingsChanged();
      return;
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
void PanelPipelineSettings::createSettings(helper::TabWidget *tab, WindowMain *windowMain)
{
  //
  //
  pipelineName = SettingBase::create<SettingLineEdit<std::string>>(windowMain, generateIcon("header"), "Pipeline name");
  pipelineName->setPlaceholderText("Name");
  pipelineName->setMaxLength(15);
  pipelineName->connectWithSetting(&mSettings.meta.name);

  //
  //
  cStackIndex = generateCStackCombo<SettingComboBox<int32_t>>("Image channel", windowMain, "Empty");
  cStackIndex->setDefaultValue(-1);
  cStackIndex->connectWithSetting(&mSettings.pipelineSetup.cStackIndex);
  connect(cStackIndex.get(), &SettingBase::valueChanged, [this]() {
    if(nullptr != mTopAddCommandButton) {
      if(cStackIndex->getValue() == -1) {
        mTopAddCommandButton->setInOutBefore(InOuts::OBJECT);
      } else {
        mTopAddCommandButton->setInOutBefore(InOuts::IMAGE);
      }
    }
  });

  //
  //
  zProjection = generateZProjection(false, windowMain);
  zProjection->connectWithSetting(&mSettings.pipelineSetup.zProjection);

  //
  //
  defaultClusterId = SettingBase::create<SettingComboBox<enums::ClusterId>>(windowMain, generateIcon("hexagon"), "Cluster");
  defaultClusterId->addOptions({{enums::ClusterId::NONE, "None"},
                                {enums::ClusterId::A, "Cluster A"},
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

  defaultClassId = SettingBase::create<SettingComboBox<enums::ClassId>>(windowMain, generateIcon("circle"), "Class");
  defaultClassId->addOptions({{enums::ClassId::UNDEFINED, "Undefined"},
                              {enums::ClassId::C0, "Class A"},
                              {enums::ClassId::C1, "Class B"},
                              {enums::ClassId::C2, "Class C"},
                              {enums::ClassId::C3, "Class D"},
                              {enums::ClassId::C4, "Class E"},
                              {enums::ClassId::C5, "Class F"},
                              {enums::ClassId::C6, "Class G"},
                              {enums::ClassId::C7, "Class H"},
                              {enums::ClassId::C8, "Class I"},
                              {enums::ClassId::C9, "Class J"}});
  defaultClassId->connectWithSetting(&mSettings.pipelineSetup.defaultClassId);

  connect(pipelineName.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::metaChangedEvent);
  connect(cStackIndex.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(zProjection.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(defaultClusterId.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(defaultClassId.get(), &joda::ui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);

  {
    auto *col1 = tab->addVerticalPanel();
    col1->addGroup("Pipeline meta", {pipelineName.get()});
    col1->addGroup("Pipeline input", {cStackIndex.get(), zProjection.get()});
    col1->addGroup("Pipeline output", {defaultClusterId.get(), defaultClassId.get()});
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
  mStopped = true;
  mPreviewQue.stop();
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

  // QObject *senderObject = sender();    // Get the signal's origin
  // if(senderObject) {
  //   qDebug() << "Signal received from:" << senderObject->objectName();
  //   senderObject->dumpObjectInfo();
  // } else {
  //   qDebug() << "Could not identify sender!";
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
  {
    std::lock_guard<std::mutex> lock(mShutingDownMutex);
    if(!mIsActiveShown) {
      return;
    }
  }
  toSettings();
  mWindowMain->checkForSettingsChanged();
  auto *controller = mWindowMain->getController();
  int cnt          = 0;
  for(const auto &myPipeline : mWindowMain->getSettings().pipelines) {
    if(&myPipeline == &getPipeline()) {
      break;
    }
    cnt++;
  }

  PreviewJob job{.settings           = mWindowMain->getSettings(),
                 .controller         = mWindowMain->getController(),
                 .previewPanel       = mPreviewImage,
                 .selectedImage      = mWindowMain->getImagePanel()->getSelectedImage(),
                 .pipelinePos        = cnt,
                 .selectedTileX      = mSelectedTileX,
                 .selectedTileY      = mSelectedTileY,
                 .clustersAndClasses = mWindowMain->getPanelClassification()->getClustersAndClasses()

  };

  std::lock_guard<std::mutex> lock(mCheckForEmptyMutex);
  mPreviewQue.push(job);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::previewThread()
{
  while(!mStopped) {
    try {
    next:
      // Wait until there is at least one job in the queue
      auto jobToDo = mPreviewQue.pop();
      // Process only the last element in the que
      if(!mPreviewQue.isEmpty()) {
        std::this_thread::sleep_for(1000ms);
        goto next;
      }
      mPreviewInProgress = true;
      emit updatePreviewStarted();
      if(nullptr != jobToDo.previewPanel && mIsActiveShown) {
        auto [imgIndex, selectedSeries] = jobToDo.selectedImage;
        if(!imgIndex.empty()) {
          try {
            int32_t resolution      = 0;
            uint32_t series         = selectedSeries;
            auto tileSize           = jobToDo.controller->getCompositeTileSize();
            auto imgProps           = joda::ctrl::Controller::getImageProperties(imgIndex, series);
            auto [tileNrX, tileNrY] = imgProps.getImageInfo().resolutions.at(resolution).getNrOfTiles(tileSize.width, tileSize.height);

            auto &previewResult = jobToDo.previewPanel->getPreviewObject();
            processor::PreviewSettings prevSettings;
            prevSettings.style = jobToDo.previewPanel->getFilledPreview() ? settings::ImageSaverSettings::Style::FILLED
                                                                          : settings::ImageSaverSettings::Style::OUTLINED;

            joda::settings::Pipeline *myPipeline = nullptr;
            int cnt                              = 0;
            for(auto &pip : jobToDo.settings.pipelines) {
              if(cnt == jobToDo.pipelinePos) {
                myPipeline = &pip;
                break;
              }
              cnt++;
            }
            if(myPipeline == nullptr) {
              continue;
            }

            jobToDo.controller->preview(jobToDo.settings.imageSetup, prevSettings, jobToDo.settings, *myPipeline, imgIndex, jobToDo.selectedTileX,
                                        jobToDo.selectedTileY, previewResult);
            // Create a QByteArray from the char array
            QString info             = "<html>";
            auto [clusters, classes] = jobToDo.clustersAndClasses;
            for(const auto &[classId, count] : previewResult.foundObjects) {
              QString tmp = "<span style=\"color: " + QString(count.color.data()) + ";\">" +
                            (clusters[classId.clusterId] + "/" + classes[classId.classId] + "</span>: " + QString::number(count.count) + "<br>");
              info += tmp;
            }
            info += "</html>";
            jobToDo.previewPanel->setThumbnailPosition(tileNrX, tileNrY, jobToDo.selectedTileX, jobToDo.selectedTileY);
            jobToDo.previewPanel->updateImage(info);

          } catch(const std::exception &error) {
            // mPreviewImage->resetImage(error.what());
            joda::log::logError("Preview error: " + std::string(error.what()));
          }
        }
      }

      {
        std::lock_guard<std::mutex> lock(mCheckForEmptyMutex);
        if(mPreviewQue.isEmpty()) {
          mPreviewInProgress = false;
          emit updatePreviewFinished();
        }
      }
    } catch(...) {
    }
  }

  mPreviewInProgress = false;
  emit updatePreviewFinished();
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
  defaultClassId->setValue(settings.pipelineSetup.defaultClassId);

  //
  // Pipelinesteps
  //
  mSettings.pipelineSteps.clear();
  for(const joda::settings::PipelineStep &step : settings.pipelineSteps) {
    mSettings.pipelineSteps.emplace_back(step);
    auto &cmdSetting                       = mSettings.pipelineSteps.back();
    std::unique_ptr<joda::ui::Command> cmd = joda::settings::PipelineFactory<joda::ui::Command>::generate(cmdSetting, mWindowMain);
    if(cmd != nullptr) {
      addPipelineStep(std::move(cmd), &cmdSetting);
    }
  }

  if(nullptr != mTopAddCommandButton) {
    if(cStackIndex->getValue() == -1) {
      mTopAddCommandButton->setInOutBefore(InOuts::OBJECT);
    } else {
      mTopAddCommandButton->setInOutBefore(InOuts::IMAGE);
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
  mSettings.pipelineSetup.defaultClassId   = defaultClassId->getValue();
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
  messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
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
  defaultClusterId->blockComponentSignals(true);
  defaultClassId->blockComponentSignals(true);

  const auto [clusters, classes] = mWindowMain->getPanelClassification()->getClustersAndClasses();

  {
    std::map<enums::ClusterId, QString> clustersN;
    for(const auto &[id, name] : clusters) {
      if(id != enums::ClusterIdIn::$) {
        clustersN.emplace(static_cast<enums::ClusterId>(id), name);
      }
    }
    defaultClusterId->changeOptionText(clustersN);
  }

  {
    std::map<enums::ClassId, QString> classN;
    for(const auto &[id, name] : classes) {
      if(id != enums::ClassIdIn::$) {
        classN.emplace(static_cast<enums::ClassId>(id), name);
      }
    }
    defaultClassId->changeOptionText(classN);
  }

  defaultClusterId->blockComponentSignals(false);
  defaultClassId->blockComponentSignals(false);

  valueChangedEvent();
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
  QString filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save template", folderToOpen,
                                                                "ImageC template files (*" + QString(joda::fs::EXT_PIPELINE_TEMPLATE.data()) + ")");
  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }

  try {
    nlohmann::json templateJson = mSettings;
    joda::templates::TemplateParser::saveTemplate(templateJson, std::filesystem::path(filePathOfSettingsFile.toStdString()));
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(mWindowMain);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not save template!");
    messageBox.setText("Could not save template, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief      Copy pipeline
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::copyPipeline()
{
  joda::settings::Pipeline copiedPipeline = mSettings;
  copiedPipeline.meta.name += " (copy)";
  mWindowMain->getPanelPipeline()->addChannel(copiedPipeline);
}

///
/// \brief      Save as template
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::setActive(bool setActive)
{
  if(!mIsActiveShown && setActive) {
    mIsActiveShown = true;
    updatePreview();
  }
  if(!setActive && mIsActiveShown) {
    std::lock_guard<std::mutex> lock(mShutingDownMutex);
    mIsActiveShown = false;
    // Wait until preview render has been finished
    while(mPreviewInProgress) {
      std::this_thread::sleep_for(100ms);
    }
    mPreviewImage->resetImage("");
  }
}

}    // namespace joda::ui
