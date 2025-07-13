///
/// \file      panel_channel.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_pipeline_settings.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qwidget.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/processor/processor.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/command/factory.hpp"
#include "ui/gui/container/container_base.hpp"
#include "ui/gui/container/dialog_command_selection/dialog_command_selection.hpp"
#include "ui/gui/container/dialog_pipeline_settings/dialog_pipeline_settings.hpp"
#include "ui/gui/container/pipeline/dialog_history.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include "ui/gui/window_main/panel_classification.hpp"
#include "ui/gui/window_main/panel_project_settings.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>
#include "add_command_button.hpp"
#include "dialog_history.hpp"

namespace joda::ui::gui {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelPipelineSettings::PanelPipelineSettings(WindowMain *wm, DialogImageViewer *previewDock, joda::settings::Pipeline &settings,
                                             std::shared_ptr<DialogCommandSelection> &commandSelectionDialog) :
    QWidget(wm),
    mLayout(this, false, true, true, false, wm), mPreviewImage(previewDock), mWindowMain(wm), mSettings(settings),
    mCommandSelectionDialog(commandSelectionDialog)
{
  setObjectName("PanelPipelineSettings");
  setContentsMargins(0, 0, 0, 0);
  auto *tab = mLayout.addTab(
      "", [] {}, false);
  mDialogHistory = new DialogHistory(wm, this);
  {
    auto *col2 = tab->addVerticalPanel();

    auto *scrollArea = new QScrollArea();
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setFrameStyle(0);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->viewport()->setStyleSheet("background-color: transparent;");
    scrollArea->setObjectName("scrollAreaOverview");
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

    col2->addWidgetGroup({scrollArea}, 300, 30000);

    // Allow to start with
    mTopAddCommandButton = new AddCommandButtonBase(mCommandSelectionDialog, mSettings, this, nullptr, InOuts::ALL, mWindowMain);
    mPipelineSteps->addWidget(mTopAddCommandButton);
  }

  // Tool button
  mUndoAction = mLayout.addActionButton("Undo", generateSvgIcon("edit-undo"));
  mUndoAction->setEnabled(false);
  mUndoAction->setStatusTip("Undo last setting");
  connect(mUndoAction, &QAction::triggered, [this]() {
    this->mDialogHistory->undo();
    mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
  });

  mHistoryAction = mLayout.addActionButton("History", generateSvgIcon("deep-history"));
  mHistoryAction->setStatusTip("Show/Hide pipeline edit history");
  mHistoryAction->setCheckable(true);
  connect(mHistoryAction, &QAction::triggered, [this](bool checked) {
    if(checked) {
      this->mDialogHistory->show();
    } else {
      mDialogHistory->hide();
    }
  });
  connect(mDialogHistory, &QDialog::finished, [this] { mHistoryAction->setChecked(false); });

  auto *addTagAction = mLayout.addActionButton("Add tag", generateSvgIcon("tag"));
  addTagAction->setStatusTip("Tag actual pipeline settings");
  addTagAction->setToolTip("Tag the actual settings in the history.");
  connect(addTagAction, &QAction::triggered, [this]() { mDialogHistory->createTag(); });

  mLayout.addSeparatorToTopToolbar();

  //
  // Add disable button
  //
  mActionDisabled = mLayout.addActionButton("Disable pipeline", generateSvgIcon("view-hidden"));
  mActionDisabled->setStatusTip("Temporary disable this pipeline");
  mActionDisabled->setCheckable(true);
  connect(mActionDisabled, &QAction::triggered, this, &PanelPipelineSettings::valueChangedEvent);

  connect(this, &PanelPipelineSettings::updatePreviewStarted, this, &PanelPipelineSettings::onPreviewStarted);
  connect(this, &PanelPipelineSettings::updatePreviewFinished, this, &PanelPipelineSettings::onPreviewFinished);
  connect(mPreviewImage, &DialogImageViewer::settingChanged, this, &PanelPipelineSettings::updatePreview);
  connect(wm->getImagePanel(), &PanelImages::imageSelectionChanged, this, &PanelPipelineSettings::updatePreview);
  connect(wm->getPanelProjectSettings(), &PanelProjectSettings::updateImagePreview, this, &PanelPipelineSettings::updatePreview);
  connect(mLayout.getBackButton(), &QAction::triggered, this, &PanelPipelineSettings::closeWindow);
  connect(wm->getPanelClassification(), &PanelClassification::settingsChanged, this, &PanelPipelineSettings::onClassificationNameChanged);
  onClassificationNameChanged();

  mPreviewThread = std::make_unique<std::thread>(&PanelPipelineSettings::previewThread, this);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::openPipelineSettings()
{
  auto *dialog = new DialogPipelineSettings(mWindowMain->getSettings().projectSettings.classification, mSettings, mWindowMain);
  if(dialog->exec() == QDialog::Accepted) {
    emit valueChangedEvent();
  }
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
  if(mCommands.empty()) {
    command->registerAddCommandButton(nullptr, mCommandSelectionDialog, mSettings, this, mWindowMain);
  } else {
    command->registerAddCommandButton(mCommands.at(mCommands.size() - 1), mCommandSelectionDialog, mSettings, this, mWindowMain);
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
  mDialogHistory->updateHistory(enums::HistoryCategory::ADDED, "Added: " + command->getTitle().toStdString());
  mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
  command->registerDeleteButton(this);

  if(mCommands.empty()) {
    command->registerAddCommandButton(nullptr, mCommandSelectionDialog, mSettings, this, mWindowMain);

  } else if(posToInsert > 0) {
    command->registerAddCommandButton(mCommands.at(posToInsert - 1), mCommandSelectionDialog, mSettings, this, mWindowMain);

  } else {
    command->registerAddCommandButton(nullptr, mCommandSelectionDialog, mSettings, this, mWindowMain);
  }

  connect(command.get(), &joda::ui::gui::Command::valueChanged, this, &PanelPipelineSettings::valueChangedEvent);
  int widgetPos = posToInsert + 1;    // Each second is a button
  mPipelineSteps->insertWidget(widgetPos, command.get());

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
void PanelPipelineSettings::erasePipelineStep(const Command *toDelete, bool updateHistoryEntry)
{
  std::string deletedCommandTitle = toDelete->getTitle().toStdString();
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
      if(updateHistoryEntry) {
        mDialogHistory->updateHistory(enums::HistoryCategory::DELETED, "Removed: " + deletedCommandTitle);
        mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
        updatePreview();
        mWindowMain->checkForSettingsChanged();
      }
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
PanelPipelineSettings::~PanelPipelineSettings()
{
  mStopped = true;
  mPreviewQue.stop();
  if(mPreviewThread != nullptr) {
    if(mPreviewThread->joinable()) {
      mPreviewThread->join();
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

  /* QObject *senderObject = sender();    // Get the signal's origin
   if(senderObject) {
     qDebug() << "Signal received from:" << senderObject->objectName();
     senderObject->dumpObjectInfo();
   } else {
     qDebug() << "Could not identify sender!";
   }*/

  if(!mLoadingSettings) {
    mDialogHistory->updateHistory(enums::HistoryCategory::CHANGED, "Changed");
  }
  mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
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
  settingsTmp.imageSetup.imageTileSettings.tileWidth  = previewSize;
  settingsTmp.imageSetup.imageTileSettings.tileHeight = previewSize;
  if(mLastSelectedPreviewSize != previewSize) {
    mLastSelectedPreviewSize = previewSize;
  }
  try {
    auto [selectedTileX, selectedTileY] = mPreviewImage->getImagePanel()->getSelectedTile();
    auto threadSettings =
        mWindowMain->getController()->calcOptimalThreadNumber(settingsTmp, std::get<2>(mWindowMain->getImagePanel()->getSelectedImage()));
    PreviewJob job{.settings       = settingsTmp,
                   .controller     = mWindowMain->getController(),
                   .previewPanel   = mPreviewImage,
                   .selectedImage  = mWindowMain->getImagePanel()->getSelectedImage(),
                   .pipelinePos    = cnt,
                   .selectedTileX  = selectedTileX,
                   .selectedTileY  = selectedTileY,
                   .timeStack      = mPreviewImage->getSelectedTimeStack(),
                   .classes        = mWindowMain->getPanelClassification()->getClasses(),
                   .classesToShow  = {},
                   .threadSettings = threadSettings};

    std::lock_guard<std::mutex> lock(mCheckForEmptyMutex);
    mPreviewQue.push(job);
    log::logTrace("Add preview job!");
  } catch(...) {
    // No image selected
  }
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
      // mCheckForEmptyMutex.lock();
      auto jobToDo = mPreviewQue.pop();
      bool isEmpty = mPreviewQue.isEmpty();
      // mCheckForEmptyMutex.unlock();

      // Process only the last element in the que
      if(!isEmpty) {
        std::this_thread::sleep_for(10ms);
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

            joda::ctrl::Preview previewResult;
            processor::PreviewSettings prevSettings;
            prevSettings.style =
                jobToDo.previewPanel->getFillOverlay() ? settings::ImageSaverSettings::Style::FILLED : settings::ImageSaverSettings::Style::OUTLINED;

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
            jobToDo.controller->preview(jobToDo.settings.imageSetup, prevSettings, jobToDo.settings, jobToDo.threadSettings, *myPipeline, imgIndex,
                                        jobToDo.selectedTileX, jobToDo.selectedTileY, jobToDo.timeStack, previewResult, imgProps,
                                        jobToDo.classesToShow);

            jobToDo.previewPanel->getImagePanel()->setOverlay(std::move(previewResult.overlay));

          } catch(const std::exception &error) {
            joda::log::logError("Preview error: " + std::string(error.what()));
          }
        }
      }

      {
        if(isEmpty) {
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
    erasePipelineStep(cmd.get(), false);
  }
  updatePreview();
  mWindowMain->checkForSettingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::pipelineSavedEvent()
{
  mDialogHistory->updateHistory(enums::HistoryCategory::SAVED, "Saved");
  mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
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
  mSettings.meta.notes    = settings.meta.notes;
  mSettings.pipelineSetup = settings.pipelineSetup;
  mSettings.setHistory(settings.getHistory(), settings.getHistoryIndex(), settings);
  mSettings.disabled = settings.disabled;
  mSettings.locked   = settings.locked;

  mActionDisabled->setChecked(settings.disabled);

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
    if(settings.pipelineSetup.cStackIndex == -1) {
      mTopAddCommandButton->setInOutBefore(InOuts::ALL);
    } else {
      mTopAddCommandButton->setInOutBefore(InOuts::ALL);
    }
  }

  QTimer::singleShot(500, this, [this]() { mLoadingSettings = false; });

  updatePreview();
  mDialogHistory->loadHistory();
  mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
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
  mSettings.disabled = mActionDisabled->isChecked();
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
/// \brief      Save as template
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::setActive(bool setActive)
{
  if(!mIsActiveShown && setActive) {
    mLayout.showToolBar(true);
    mIsActiveShown = true;
    updatePreview();
    mDialogHistory->loadHistory();
    mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
  }
  if(!setActive && mIsActiveShown) {
    std::lock_guard<std::mutex> lock(mShutingDownMutex);
    mIsActiveShown = false;
    mLayout.showToolBar(false);
    mPreviewImage->getImagePanel()->clearOverlay();
    mDialogHistory->hide();
    mHistoryAction->setChecked(false);
    // Wait until preview render has been finished
    while(mPreviewInProgress) {
      std::this_thread::sleep_for(100ms);
    }
  }
}

}    // namespace joda::ui::gui
