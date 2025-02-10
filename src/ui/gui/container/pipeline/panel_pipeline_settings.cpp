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
#include "backend/helper/logger/console_logger.hpp"
#include "backend/processor/processor.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/command/factory.hpp"
#include "ui/gui/container/container_base.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include "ui/gui/window_main/panel_classification.hpp"
#include "ui/gui/window_main/panel_project_settings.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>
#include "add_command_button.hpp"

namespace joda::ui::gui {

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
  auto *tab = mLayout.addTab(
      "", [] {}, false);
  createSettings(tab, wm);

  {
    auto *col2 = tab->addVerticalPanel();

    auto *scrollArea = new QScrollArea();
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
    mTopAddCommandButton = new AddCommandButtonBase(mSettings, this, nullptr, InOuts::ALL, mWindowMain);
    mPipelineSteps->addWidget(mTopAddCommandButton);
  }

  {
    auto *col3 = tab->addVerticalPanel();
  }

  {
    auto *col4    = tab->addVerticalPanel();
    mPreviewImage = new PanelPreview(PREVIEW_BASE_SIZE, PREVIEW_BASE_SIZE, mWindowMain);
    mPreviewImage->setContentsMargins(0, 0, 0, 0);
    mPreviewImage->resetImage("");
    col4->addWidget(mPreviewImage);
  }

  // Tool button
  auto *openTemplate = mLayout.addActionButton("Open template", generateIcon("opened-folder"));
  connect(openTemplate, &QAction::triggered, [this] { this->openTemplate(); });

  auto *saveAsTemplateButton = mLayout.addActionButton("Save as template", generateIcon("bookmark"));
  connect(saveAsTemplateButton, &QAction::triggered, [this] { this->saveAsTemplate(); });

  auto *copyPipeline = mLayout.addActionButton("Copy pipeline", generateIcon("copy"));
  connect(copyPipeline, &QAction::triggered, [this] { this->copyPipeline(); });

  connect(this, &PanelPipelineSettings::updatePreviewStarted, this, &PanelPipelineSettings::onPreviewStarted);
  connect(this, &PanelPipelineSettings::updatePreviewFinished, this, &PanelPipelineSettings::onPreviewFinished);
  connect(mPreviewImage, &PanelPreview::tileClicked, this, &PanelPipelineSettings::onTileClicked);
  connect(mPreviewImage, &PanelPreview::onSettingChanged, this, &PanelPipelineSettings::updatePreview);
  connect(wm->getImagePanel(), &PanelImages::imageSelectionChanged, this, &PanelPipelineSettings::updatePreview);
  connect(wm->getPanelProjectSettings(), &PanelProjectSettings::updateImagePreview, this, &PanelPipelineSettings::updatePreview);
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
void PanelPipelineSettings::addPipelineStep(std::unique_ptr<joda::ui::gui::Command> command, const settings::PipelineStep *pipelineStepBefore)
{
  command->registerDeleteButton(this);
  command->registerAddCommandButton(mSettings, this, mWindowMain);
  if(mCommands.empty()) {
    command->setCommandBefore(nullptr);
  } else {
    command->setCommandBefore(mCommands.at(mCommands.size() - 1));
  }
  connect(command.get(), &joda::ui::gui::Command::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  mPipelineSteps->addWidget(command.get());
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
void PanelPipelineSettings::insertNewPipelineStep(int32_t posToInsert, std::unique_ptr<joda::ui::gui::Command> command,
                                                  const settings::PipelineStep *pipelineStepBefore)
{
  command->registerDeleteButton(this);
  command->registerAddCommandButton(mSettings, this, mWindowMain);
  connect(command.get(), &joda::ui::gui::Command::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  int widgetPos = posToInsert + 1;    // Each second is a button
  mPipelineSteps->insertWidget(widgetPos, command.get());

  if(mCommands.empty()) {
    command->setCommandBefore(nullptr);
  } else if(posToInsert > 0) {
    command->setCommandBefore(mCommands.at(posToInsert - 1));
  } else {
    command->setCommandBefore(nullptr);
  }
  mCommands.insert(mCommands.begin() + posToInsert, std::move(command));

  if((posToInsert + 1) < mCommands.size()) {
    mCommands.at(posToInsert + 1)->setCommandBefore(mCommands.at(posToInsert));
  }

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
        int32_t deletedPos = 0;
        for(auto it = mCommands.begin(); it != mCommands.end(); it++) {
          if(it->get() == toDelete) {
            mCommands.erase(it);
            break;
          }
          deletedPos++;
        }

        // This command is now at the old position. And for this command the parent has been changed
        if(deletedPos < mCommands.size()) {
          int32_t posPrev                  = deletedPos - 1;
          int32_t posNext                  = deletedPos + 1;
          std::shared_ptr<Command> &newOld = mCommands.at(deletedPos);
          if(posPrev >= 0) {
            std::shared_ptr<Command> &prevCommand = mCommands.at(posPrev);
            newOld->setCommandBefore(prevCommand);
          } else {
            newOld->setCommandBefore(nullptr);
          }
          if(posNext < mCommands.size()) {
            mCommands.at(posNext)->setCommandBefore(newOld);
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
        mTopAddCommandButton->setInOutBefore(InOuts::ALL);
      } else {
        mTopAddCommandButton->setInOutBefore(InOuts::ALL);
      }
    }
  });

  //
  //
  zProjection = generateZProjection(false, windowMain);
  zProjection->connectWithSetting(&mSettings.pipelineSetup.zProjection);

  //
  //
  defaultClassId = SettingBase::create<SettingComboBoxClassesOutN>(windowMain, generateIcon("circle"), "Class");
  defaultClassId->addOptions({
      {enums::ClassId::UNDEFINED, "Undefined"}, {enums::ClassId::C0, "Class C0"},   {enums::ClassId::C1, "Class C1"},
      {enums::ClassId::C2, "Class C2"},         {enums::ClassId::C3, "Class C3"},   {enums::ClassId::C4, "Class C4"},
      {enums::ClassId::C5, "Class C5"},         {enums::ClassId::C6, "Class C6"},   {enums::ClassId::C7, "Class C7"},
      {enums::ClassId::C8, "Class C8"},         {enums::ClassId::C9, "Class C9"},   {enums::ClassId::C10, "Class C10"},
      {enums::ClassId::C11, "Class C11"},       {enums::ClassId::C12, "Class C12"}, {enums::ClassId::C13, "Class C13"},
      {enums::ClassId::C14, "Class C14"},       {enums::ClassId::C15, "Class C15"}, {enums::ClassId::C16, "Class C16"},
      {enums::ClassId::C17, "Class C17"},       {enums::ClassId::C18, "Class C18"}, {enums::ClassId::C19, "Class C19"},
      {enums::ClassId::C20, "Class C20"},       {enums::ClassId::C21, "Class C21"}, {enums::ClassId::C22, "Class C22"},
      {enums::ClassId::C23, "Class C23"},       {enums::ClassId::C24, "Class C24"}, {enums::ClassId::C25, "Class C25"},
      {enums::ClassId::C26, "Class C26"},       {enums::ClassId::C27, "Class C27"}, {enums::ClassId::C28, "Class C28"},
      {enums::ClassId::C29, "Class C29"},       {enums::ClassId::C30, "Class C30"}, {enums::ClassId::C31, "Class C31"},
      {enums::ClassId::C32, "Class C32"},
  });
  defaultClassId->connectWithSetting(&mSettings.pipelineSetup.defaultClassId);
  defaultClassId->classsNamesChanged();

  connect(pipelineName.get(), &joda::ui::gui::SettingBase::valueChanged, this, &PanelPipelineSettings::metaChangedEvent);
  connect(cStackIndex.get(), &joda::ui::gui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(zProjection.get(), &joda::ui::gui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  connect(defaultClassId.get(), &joda::ui::gui::SettingBase::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);

  {
    auto *col1 = tab->addVerticalPanel();
    col1->addGroup("Pipeline meta", {pipelineName.get()});
    col1->addGroup("Pipeline input", {cStackIndex.get(), zProjection.get()});
    col1->addGroup("Pipeline output", {defaultClassId.get()});
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
  static bool isBlocked = false;
  // if(cStackIndex < 0) {
  //   zProjection->getDisplayLabelWidget()->setVisible(false);
  // } else {
  //   zProjection->getDisplayLabelWidget()->setVisible(true);
  // }

  if(isBlocked) {
    return;
  }

  isBlocked = true;
  /*
    QObject *senderObject = sender();    // Get the signal's origin
    if(senderObject) {
      qDebug() << "Signal received from:" << senderObject->objectName();
      senderObject->dumpObjectInfo();
    } else {
      qDebug() << "Could not identify sender!";
    }
    */
  updatePreview();

  QTimer::singleShot(100, this, [this]() {
    isBlocked = false;    // Unblock after 100ms
  });
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
    if(!mIsActiveShown || mLoadingSettings) {
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

  settings::AnalyzeSettings settingsTmp = mWindowMain->getSettings();

  auto previewSize                                    = mPreviewImage->getPreviewSize();
  auto classesToShow                                  = mPreviewImage->getSelectedClassesAndClasses();
  settingsTmp.imageSetup.imageTileSettings.tileWidth  = previewSize;
  settingsTmp.imageSetup.imageTileSettings.tileHeight = previewSize;
  if(mLastSelectedPreviewSize != previewSize) {
    mLastSelectedPreviewSize = previewSize;
    mSelectedTileX           = 0;
    mSelectedTileY           = 0;
  }

  PreviewJob job{.settings      = settingsTmp,
                 .controller    = mWindowMain->getController(),
                 .previewPanel  = mPreviewImage,
                 .selectedImage = mWindowMain->getImagePanel()->getSelectedImage(),
                 .pipelinePos   = cnt,
                 .selectedTileX = mSelectedTileX,
                 .selectedTileY = mSelectedTileY,
                 .classes       = mWindowMain->getPanelClassification()->getClasses(),
                 .classesToShow = classesToShow};

  std::lock_guard<std::mutex> lock(mCheckForEmptyMutex);
  mPreviewQue.push(job);
  log::logTrace("Add preview job!");
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
        auto [imgIndex, selectedSeries, imgProps] = jobToDo.selectedImage;
        if(!imgIndex.empty()) {
          try {
            int32_t resolution = 0;
            int32_t series     = selectedSeries;
            auto tileSize      = jobToDo.settings.imageSetup.imageTileSettings;

            // If image is too big scale to tiles
            auto imgWidth    = imgProps.getImageInfo(series).resolutions.at(0).imageWidth;
            auto imageHeight = imgProps.getImageInfo(series).resolutions.at(0).imageHeight;

            if(imgWidth > jobToDo.settings.imageSetup.imageTileSettings.tileWidth ||
               imageHeight > jobToDo.settings.imageSetup.imageTileSettings.tileHeight) {
              tileSize.tileWidth  = jobToDo.settings.imageSetup.imageTileSettings.tileWidth;
              tileSize.tileHeight = jobToDo.settings.imageSetup.imageTileSettings.tileHeight;
            } else {
              tileSize.tileWidth  = imgWidth;
              tileSize.tileHeight = imageHeight;
            }

            auto [tileNrX, tileNrY] = imgProps.getImageInfo(series).resolutions.at(resolution).getNrOfTiles(tileSize.tileWidth, tileSize.tileHeight);

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
                                        jobToDo.selectedTileY, previewResult, imgProps, jobToDo.classesToShow);
            // Create a QByteArray from the char array
            QString info = "<html>";
            auto classes = jobToDo.classes;
            for(const auto &[classId, count] : previewResult.foundObjects) {
              QString tmp = "<span style=\"color: " + QString(count.color.data()) + ";\">" +
                            (classes[static_cast<enums::ClassIdIn>(classId)] + "</span>: " + QString::number(count.count) + "<br>");
              info += tmp;
            }
            info += "</html>";
            jobToDo.previewPanel->setThumbnailPosition(
                PanelImageView::ThumbParameter{.nrOfTilesX          = tileNrX,
                                               .nrOfTilesY          = tileNrY,
                                               .tileWidth           = jobToDo.settings.imageSetup.imageTileSettings.tileWidth,
                                               .tileHeight          = jobToDo.settings.imageSetup.imageTileSettings.tileHeight,
                                               .originalImageWidth  = imgWidth,
                                               .originalImageHeight = imageHeight,
                                               .selectedTileX       = jobToDo.selectedTileX,
                                               .selectedTileY       = jobToDo.selectedTileY});
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
void PanelPipelineSettings::clearPipeline()
{
  std::vector<std::shared_ptr<Command>> toDelete = mCommands;
  for(const std::shared_ptr<Command> &cmd : toDelete) {
    erasePipelineStep(cmd.get());
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
  mLoadingSettings        = true;
  mSettings.meta          = settings.meta;
  mSettings.pipelineSetup = settings.pipelineSetup;

  pipelineName->setValue(settings.meta.name);
  cStackIndex->setValue(settings.pipelineSetup.cStackIndex);
  zProjection->setValue(settings.pipelineSetup.zProjection);
  defaultClassId->setValue(settings.pipelineSetup.defaultClassId);

  //
  // Pipelinesteps
  //
  mSettings.pipelineSteps.clear();
  for(const joda::settings::PipelineStep &step : settings.pipelineSteps) {
    mSettings.pipelineSteps.emplace_back(step);
    auto &cmdSetting                            = mSettings.pipelineSteps.back();
    std::unique_ptr<joda::ui::gui::Command> cmd = joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(cmdSetting, mWindowMain);
    if(cmd != nullptr) {
      addPipelineStep(std::move(cmd), &cmdSetting);
    }
  }

  if(nullptr != mTopAddCommandButton) {
    if(cStackIndex->getValue() == -1) {
      mTopAddCommandButton->setInOutBefore(InOuts::ALL);
    } else {
      mTopAddCommandButton->setInOutBefore(InOuts::ALL);
    }
  }
  mLoadingSettings = false;

  updatePreview();
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
  mSettings.meta.name                    = pipelineName->getValue();
  mSettings.pipelineSetup.cStackIndex    = cStackIndex->getValue();
  mSettings.pipelineSetup.zProjection    = zProjection->getValue();
  mSettings.pipelineSetup.defaultClassId = defaultClassId->getValue();
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
///               Class or Classs with the new name
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::onClassificationNameChanged()
{
  valueChangedEvent();
}

///
/// \brief      Open template
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::openTemplate()
{
  QString folderToOpen           = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
  QString filePathOfSettingsFile = QFileDialog::getOpenFileName(this, "Open template", folderToOpen,
                                                                "ImageC template files (*" + QString(joda::fs::EXT_PIPELINE_TEMPLATE.data()) + ")");
  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }

  try {
    nlohmann::json templateJson = mSettings;
    auto loadedChannelSettings  = joda::templates::TemplateParser::loadTemplate(std::filesystem::path(filePathOfSettingsFile.toStdString()));
    clearPipeline();
    fromSettings(loadedChannelSettings);
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(mWindowMain);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not open template!");
    messageBox.setText("Could not open template, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
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

}    // namespace joda::ui::gui
