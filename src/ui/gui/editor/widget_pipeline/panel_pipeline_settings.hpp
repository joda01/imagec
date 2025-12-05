///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qboxlayout.h>
#include <qtoolbar.h>
#include <QtWidgets>
#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/thread_safe_queue.hpp"
#include "backend/settings/analze_settings.hpp"
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classes_out.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_spinbox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_text_edit.hpp"
#include "ui/gui/helper/layout_generator.hpp"

class PlaceholderTableView;

namespace joda::settings {
class PipelineHistoryEntry;
};

namespace joda::ui::gui {

class DialogHistory;
class WindowMain;
class AddCommandButtonBase;
class PanelClassification;
class DialogCommandSelection;
class DialogImageViewer;
class DialogRoiManager;
class DialogMlTrainer;

class PanelPipelineSettings : public QWidget
{
  Q_OBJECT

signals:
  void updatePreviewStarted();
  void updatePreviewFinished(QString error);

public:
  PanelPipelineSettings(WindowMain *wm, DialogImageViewer *previewDock, joda::processor::Preview *previewResult, joda::settings::Pipeline &settings,
                        std::shared_ptr<DialogCommandSelection> &commandSelectionDialog, DialogMlTrainer *mlTraining);
  ~PanelPipelineSettings();

  void addPipelineStep(std::unique_ptr<joda::ui::gui::Command> command, const settings::PipelineStep *);
  void insertNewPipelineStep(size_t posToInsert, std::unique_ptr<joda::ui::gui::Command> command, const settings::PipelineStep *pipelineStepBefore);
  void erasePipelineStep(const Command *, bool updateHistory = true);
  void setActive(bool setActive);
  const joda::settings::Pipeline &getPipeline()
  {
    return mSettings;
  }

  joda::settings::Pipeline &mutablePipeline()
  {
    return mSettings;
  }

  QWidget *getEditPanel()
  {
    return this;
  }

  nlohmann::json toJson(const std::string & /*titlePrefix*/)
  {
    return {};
  }

  void toSettings();
  void fromSettings(const joda::settings::Pipeline &settings);
  void clearPipeline();
  void pipelineSavedEvent();
  void openPipelineSettings();
  void triggerPreviewUpdate();
  auto getListOfCommands() -> std::vector<std::shared_ptr<Command>> *
  {
    return &mCommands;
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  void previewThread();
  void setImageMustBeRefreshed(bool);

  // ACTIONS///////////////////////////////////////////////////
  QAction *mRefresh = nullptr;
  QAction *mUndoAction;
  QAction *mHistoryAction;
  QAction *mActionDisabled;
  QAction *mActionEditMode;

  /////////////////////////////////////////////////////
  QToolBar *mToolbar;
  QVBoxLayout *mLayout;

  /////////////////////////////////////////////////////
  DialogHistory *mDialogHistory;
  DialogImageViewer *mPreviewImage = nullptr;
  DialogMlTrainer *mMlTraining;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;
  std::atomic<bool> mPreviewInProgress        = false;
  bool mLoadingSettings                       = false;
  WindowMain *mWindowMain;
  AddCommandButtonBase *mTopAddCommandButton;
  std::shared_ptr<DialogCommandSelection> mCommandSelectionDialog;

  // PIPELINE STEPS //////////////////////////////////////////////////
  QVBoxLayout *mPipelineSteps;
  std::vector<std::shared_ptr<Command>> mCommands;

  /////////////////////////////////////////////////////
  int32_t mLastSelectedPreviewSize = 0;
  joda::settings::Pipeline &mSettings;
  joda::processor::Preview *mPreviewResult;
  bool mStopped                                         = false;
  std::atomic<bool> mTriggerPreviewUpdate               = false;
  std::atomic<int32_t> mNumberOfChangesSinceLastRefresh = 0;
  std::mutex mCheckForEmptyMutex;
  std::mutex mShutingDownMutex;

  QMetaObject::Connection mImageOpenedConnection;
  QMetaObject::Connection mTrainingFinishedConnection;

private slots:
  /////////////////////////////////////////////////////
  void onPreviewStarted();
  void onPreviewFinished(QString error);
  void metaChangedEvent();
  void closeWindow();
  void onClassificationNameChanged();
};

}    // namespace joda::ui::gui
