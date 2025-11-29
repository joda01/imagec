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
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/uuid.hpp"
#include "backend/processor/processor.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/gui/dialogs/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/dialogs/dialog_ml_trainer/dialog_ml_trainer.hpp"
#include "ui/gui/editor/widget_pipeline/dialog_command_selection/dialog_command_selection.hpp"
#include "ui/gui/editor/widget_pipeline/dialog_history/dialog_history.hpp"
#include "ui/gui/editor/widget_pipeline/dialog_pipeline_settings/dialog_pipeline_settings.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/factory.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_classification.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_image.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_project_settings.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
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
PanelPipelineSettings::PanelPipelineSettings(WindowMain *wm, DialogImageViewer *previewDock, joda::processor::Preview *previewResult,
                                             joda::settings::Pipeline &settings, std::shared_ptr<DialogCommandSelection> &commandSelectionDialog,
                                             DialogMlTrainer *mlTraining) :
    QWidget(wm),
    mPreviewImage(previewDock), mMlTraining(mlTraining), mWindowMain(wm), mCommandSelectionDialog(commandSelectionDialog), mSettings(settings),
    mPreviewResult(previewResult)
{
  setObjectName("PanelPipelineSettings");
  setContentsMargins(0, 0, 0, 0);

  mLayout  = new QVBoxLayout();
  mToolbar = new QToolBar();
  CHECK_GUI_THREAD(mToolbar)
  mToolbar->setVisible(false);
  mToolbar->setMovable(false);
  wm->addToolBar(Qt::ToolBarArea::TopToolBarArea, mToolbar);

  mDialogHistory = new DialogHistory(wm, &settings);
  {
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

    mLayout->addWidget(scrollArea);

    // Allow to start with
    mTopAddCommandButton = new AddCommandButtonBase(mCommandSelectionDialog, mSettings, this, nullptr, InOuts::ALL, mWindowMain);
    mPipelineSteps->addWidget(mTopAddCommandButton);
  }

  // Tool button
  mToolbar->addSeparator();

  //
  // Refresh
  //
  mRefresh = mToolbar->addAction(generateSvgIcon<Style::REGULAR, Color::RED>("arrows-clockwise"), "Auto refresh preview");
  mRefresh->setStatusTip("Automatic preview refresh");
  mRefresh->setCheckable(true);

  //
  // Switch to edit mode
  //
  mActionEditMode = mToolbar->addAction(generateSvgIcon<Style::REGULAR, Color::RED>("eye"), "Live mode");
  mActionEditMode->setStatusTip("Switch to live edit mode");
  mActionEditMode->setCheckable(true);
  connect(mActionEditMode, &QAction::triggered, [this](bool selected) { mPreviewImage->getImagePanel()->setShowEditedImage(selected); });

  //
  // Add disable button
  //
  mActionDisabled = mToolbar->addAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("selection-slash"), "Disable pipeline");
  mActionDisabled->setStatusTip("Temporary disable this pipeline");
  mActionDisabled->setCheckable(true);

  mToolbar->addSeparator();

  //
  // Undo
  //
  mUndoAction = mToolbar->addAction(generateSvgIcon<Style::REGULAR, Color::RED>("arrow-counter-clockwise"), "Undo");
  CHECK_GUI_THREAD(mUndoAction)
  mUndoAction->setEnabled(false);
  mUndoAction->setStatusTip("Undo last setting");
  connect(mUndoAction, &QAction::triggered, [this]() {
    mSettings.undo();
    CHECK_GUI_THREAD(mUndoAction)
    mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
  });
  settings.registerHistoryChangeCallback([this] {
    CHECK_GUI_THREAD(mUndoAction)
    mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
  });
  settings.registerSnapShotRestored([this](const joda::settings::Pipeline &pip) {
    clearPipeline();
    fromSettings(pip);
  });

  //
  // History
  //
  mHistoryAction = mToolbar->addAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("clock-counter-clockwise"), "History");
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

  auto *addTagAction = mToolbar->addAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("tag-simple"), "Add tag");
  addTagAction->setStatusTip("Tag actual pipeline settings");
  addTagAction->setToolTip("Tag the actual settings in the history.");
  connect(addTagAction, &QAction::triggered, [this]() {
    QInputDialog inputDialog(mWindowMain);
    inputDialog.setWindowTitle("Create tag");
    inputDialog.setLabelText("Tag name:");
    inputDialog.setInputMode(QInputDialog::TextInput);
    auto ret = inputDialog.exec();
    if(QInputDialog::Accepted == ret) {
      QString text = inputDialog.textValue();
      if(!text.isEmpty()) {
        mSettings.tag(text.toStdString());
      }
    }
  });

  mToolbar->addSeparator();

  //
  // Close button
  //
  auto *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mToolbar->addWidget(spacer);

  auto *closePipeline = mToolbar->addAction(generateSvgIcon<Style::REGULAR, Color::RED>("x"), "Close pipeline editor");
  closePipeline->setStatusTip("Close pipeline editor");

  //
  // Preview dialog
  //
  setLayout(mLayout);
  connect(this, &PanelPipelineSettings::updatePreviewStarted, this, &PanelPipelineSettings::onPreviewStarted);
  connect(this, &PanelPipelineSettings::updatePreviewFinished, this, &PanelPipelineSettings::onPreviewFinished);
  connect(closePipeline, &QAction::triggered, this, &PanelPipelineSettings::closeWindow);
  connect(wm->getPanelClassification(), &PanelClassification::settingsChanged, this, &PanelPipelineSettings::onClassificationNameChanged);
  onClassificationNameChanged();

  mPreviewThread = std::make_unique<std::thread>(&PanelPipelineSettings::previewThread, this);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mSettings.registerPipelineChangedCallback([this](const settings::Pipeline &) { setImageMustBeRefreshed(true); });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::setImageMustBeRefreshed(bool refresh)
{
  if(mIsActiveShown) {
    if(refresh) {
      mNumberOfChangesSinceLastRefresh.fetch_add(1, std::memory_order_relaxed);
      if(mRefresh != nullptr && !mRefresh->isChecked()) {
        mPreviewImage->getImagePanel()->setInfoText("Press F5 to update preview (" + std::to_string(mNumberOfChangesSinceLastRefresh) + ")");
      }
    } else {
      mPreviewImage->getImagePanel()->setInfoText("");
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
void PanelPipelineSettings::openPipelineSettings()
{
  auto *dialog = new DialogPipelineSettings(mWindowMain->getSettings().projectSettings.classification, mSettings, mWindowMain);
  if(dialog->exec() == QDialog::Accepted) {
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::addPipelineStep(std::unique_ptr<joda::ui::gui::Command> command, const settings::PipelineStep * /*pipelineStepBefore*/)
{
  command->registerDeleteButton(this);
  if(mCommands.empty()) {
    command->registerAddCommandButton(nullptr, mCommandSelectionDialog, mSettings, this, mWindowMain);
  } else {
    command->registerAddCommandButton(mCommands.at(mCommands.size() - 1), mCommandSelectionDialog, mSettings, this, mWindowMain);
  }
  mPipelineSteps->addWidget(command.get());
  mCommands.push_back(std::move(command));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipelineSettings::insertNewPipelineStep(size_t posToInsert, std::unique_ptr<joda::ui::gui::Command> command,
                                                  const settings::PipelineStep * /*pipelineStepBefore*/)
{
  mSettings.createSnapShot(enums::HistoryCategory::ADDED, "Added: " + command->getTitle().toStdString());
  mSettings.triggerPipelineChanged();
  CHECK_GUI_THREAD(mUndoAction)
  mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
  command->registerDeleteButton(this);

  if(mCommands.empty()) {
    command->registerAddCommandButton(nullptr, mCommandSelectionDialog, mSettings, this, mWindowMain);
  } else if(posToInsert > 0) {
    command->registerAddCommandButton(mCommands.at(posToInsert - 1), mCommandSelectionDialog, mSettings, this, mWindowMain);
  } else {
    command->registerAddCommandButton(nullptr, mCommandSelectionDialog, mSettings, this, mWindowMain);
  }

  size_t widgetPos = posToInsert + 1;    // Each second is a button
  mPipelineSteps->insertWidget(static_cast<int>(widgetPos), command.get());

  mCommands.insert(mCommands.begin() + static_cast<int>(posToInsert), std::move(command));

  if((posToInsert + 1) < mCommands.size()) {
    mCommands.at(posToInsert + 1)->setCommandBefore(mCommands.at(posToInsert));
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
        if(deletedPos < static_cast<int32_t>(mCommands.size())) {
          int32_t posPrev                  = deletedPos - 1;
          int32_t posNext                  = deletedPos + 1;
          std::shared_ptr<Command> &newOld = mCommands.at(static_cast<size_t>(deletedPos));
          if(posPrev >= 0) {
            std::shared_ptr<Command> &prevCommand = mCommands.at(static_cast<size_t>(posPrev));
            newOld->setCommandBefore(prevCommand);
          } else {
            newOld->setCommandBefore(nullptr);
          }
          if(posNext < static_cast<int32_t>(mCommands.size())) {
            mCommands.at(static_cast<size_t>(posNext))->setCommandBefore(newOld);
          }
        }
      }
      if(updateHistoryEntry) {
        mSettings.createSnapShot(enums::HistoryCategory::DELETED, "Removed: " + deletedCommandTitle);
        mSettings.triggerPipelineChanged();
        CHECK_GUI_THREAD(mUndoAction)
        mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
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
void PanelPipelineSettings::triggerPreviewUpdate()
{
  mTriggerPreviewUpdate = true;
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
    QString errorMsg;
    int32_t numberOfChangesSinceLastRefresh = mNumberOfChangesSinceLastRefresh;
    if(numberOfChangesSinceLastRefresh > 0 && mIsActiveShown && (mRefresh->isChecked() || mTriggerPreviewUpdate)) {
      mPreviewInProgress = true;
      emit updatePreviewStarted();
      try {
        // Collect data
        settings::AnalyzeSettings settingsTmp           = mWindowMain->getSettings();
        auto *controller                                = mWindowMain->getController();
        const auto [imgIndex, selectedSeries, imgProps] = mWindowMain->getImagePanel()->getSelectedImage();
        const auto imgWidth                             = imgProps.getImageInfo(selectedSeries).resolutions.at(0).imageWidth;
        const auto imageHeight                          = imgProps.getImageInfo(selectedSeries).resolutions.at(0).imageHeight;
        const auto [selectedTileX, selectedTileY]       = mPreviewImage->getImagePanel()->getSelectedTile();
        const auto timeStack                            = mPreviewImage->getSelectedTimeStack();
        const auto threadSettings                       = mWindowMain->getController()->calcOptimalThreadNumber(settingsTmp, imgProps);
        const joda::settings::Pipeline *myPipeline      = nullptr;
        for(auto &pip : settingsTmp.pipelines) {
          if(!pip.meta.uid.empty() && pip.meta.uid == getPipeline().meta.uid) {
            myPipeline = &pip;
            break;
          }
        }

        if(!imgIndex.empty()) {
          auto tileSize = settingsTmp.imageSetup.imageTileSettings;
          // If image is too big scale to tiles
          if(imgWidth > settingsTmp.imageSetup.imageTileSettings.tileWidth || imageHeight > settingsTmp.imageSetup.imageTileSettings.tileHeight) {
            tileSize.tileWidth  = settingsTmp.imageSetup.imageTileSettings.tileWidth;
            tileSize.tileHeight = settingsTmp.imageSetup.imageTileSettings.tileHeight;
          } else {
            tileSize.tileWidth  = imgWidth;
            tileSize.tileHeight = imageHeight;
          }
          if(myPipeline != nullptr) {
            controller->preview(settingsTmp.imageSetup, processor::PreviewSettings{}, settingsTmp, threadSettings, *myPipeline, imgIndex,
                                selectedTileX, selectedTileY, timeStack, *mPreviewResult, imgProps);

          } else {
            errorMsg = "No images found! Select working directory.";
          }
        }
      } catch(const std::exception &error) {
        errorMsg = QString(error.what()) + QString("\n\nSee compiler log and application log for more details!");
      }

      // We subtract the number of changed at startup from the actual number.
      // If during the preview generation some settings were taken the counter is not null and we have to refresh again
      mNumberOfChangesSinceLastRefresh.fetch_sub(numberOfChangesSinceLastRefresh, std::memory_order_relaxed);
      mPreviewInProgress    = false;
      mTriggerPreviewUpdate = false;
      emit updatePreviewFinished(errorMsg);
    }

    std::this_thread::sleep_for(100ms);
  }
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
void PanelPipelineSettings::onPreviewFinished(QString error)
{
  if(!error.isEmpty()) {
    mRefresh->setChecked(false);
    QMessageBox messageBox(mWindowMain);
    messageBox.setIconPixmap(generateSvgIcon<Style::DUETONE, Color::RED>("warning-octagon").pixmap(48, 48));
    messageBox.setWindowTitle("Pipeline error");
    messageBox.setText(error);
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    messageBox.exec();
  } else {
    mPreviewImage->getImagePanel()->setEditedImage(std::move(mPreviewResult->editedImage));
    mPreviewImage->getImagePanel()->setRegionsOfInterestFromObjectList();
    mPreviewImage->getImagePanel()->repaintImage();

    setImageMustBeRefreshed(false);
  }
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
  mSettings.createSnapShot(enums::HistoryCategory::SAVED, "Saved");
  CHECK_GUI_THREAD(mUndoAction)
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
  mLoadingSettings = true;
  mSettings.meta   = settings.meta;
  if(mSettings.meta.uid.empty()) {
    mSettings.meta.uid = joda::helper::generate_uuid();
  }
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
    auto &cmdSetting = mSettings.pipelineSteps.back();
    std::unique_ptr<joda::ui::gui::Command> cmd =
        joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(&mWindowMain->mutableSettings(), cmdSetting, mWindowMain);
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
  mWindowMain->checkForSettingsChanged();

  QTimer::singleShot(500, this, [this]() { mLoadingSettings = false; });
  CHECK_GUI_THREAD(mUndoAction)
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
  mPreviewImage->getImagePanel()->setRegionsOfInterestFromObjectList();
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
    // Set the image channel initial to the selected channel of the pipeline
    if(mSettings.pipelineSetup.cStackIndex >= 0) {
      if(mPreviewImage->getImagePanel()->getImagePlane().cStack != mSettings.pipelineSetup.cStackIndex) {
        mPreviewImage->getImagePanel()->setImageChannel(mSettings.pipelineSetup.cStackIndex);
        mPreviewImage->getImagePanel()->reloadImage();
      }
    }
    CHECK_GUI_THREAD(mToolbar)
    mToolbar->setVisible(true);
    mIsActiveShown = true;
    setImageMustBeRefreshed(true);
    CHECK_GUI_THREAD(mUndoAction)
    mUndoAction->setEnabled(mSettings.getHistoryIndex() + 1 < mSettings.getHistory().size());
    mPreviewImage->getImagePanel()->setShowEditedImage(mActionEditMode->isChecked());

    mImageOpenedConnection      = connect(mPreviewImage->getImagePanel(), &PanelImageView::imageOpened, [this]() { setImageMustBeRefreshed(true); });
    mTrainingFinishedConnection = connect(mMlTraining, &DialogMlTrainer::trainingFinished, [this]() { setImageMustBeRefreshed(true); });
  }
  if(!setActive && mIsActiveShown) {
    disconnect(mImageOpenedConnection);
    disconnect(mTrainingFinishedConnection);
    mRefresh->setChecked(false);
    mPreviewResult->results.objectMap->triggerStartChangeCallback();
    mPreviewResult->results.objectMap->erase(joda::atom::ROI::Category::AUTO_SEGMENTATION);
    mPreviewResult->results.objectMap->triggerChangeCallback();
    mPreviewImage->getImagePanel()->setRegionsOfInterestFromObjectList();
    std::lock_guard<std::mutex> lock(mShutingDownMutex);
    setImageMustBeRefreshed(false);
    mIsActiveShown = false;
    CHECK_GUI_THREAD(mToolbar)
    mToolbar->setVisible(false);
    mPreviewImage->getImagePanel()->setShowEditedImage(false);
    mDialogHistory->hide();
    mHistoryAction->setChecked(false);
    // Wait until preview render has been finished
    while(mPreviewInProgress) {
      std::this_thread::sleep_for(100ms);
    }
  }
}

}    // namespace joda::ui::gui
