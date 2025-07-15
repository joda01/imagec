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
#include <memory>
#include <mutex>
#include <optional>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/thread_safe_queue.hpp"
#include "backend/settings/analze_settings.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/container_base.hpp"
#include "ui/gui/container/dialog_preview_results/dialog_preview_results.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/container/setting/setting_spinbox.hpp"
#include "ui/gui/container/setting/setting_text_edit.hpp"
#include "ui/gui/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/helper/layout_generator.hpp"

class PlaceholderTableWidget;

namespace joda::settings {
class PipelineHistoryEntry;
};

namespace joda::ui::gui {

class DialogHistory;
class WindowMain;
class AddCommandButtonBase;
class PanelClassification;
class DialogCommandSelection;
class DialogPreviewResults;

class PanelPipelineSettings : public QWidget, public ContainerBase
{
  Q_OBJECT

  friend class DialogPreviewResults;

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();

public:
  PanelPipelineSettings(WindowMain *wm, DialogImageViewer *previewDock, DialogPreviewResults *previewResults, joda::settings::Pipeline &settings,
                        std::shared_ptr<DialogCommandSelection> &commandSelectionDialog);
  ~PanelPipelineSettings();

  void addPipelineStep(std::unique_ptr<joda::ui::gui::Command> command, const settings::PipelineStep *);
  void insertNewPipelineStep(int32_t posToInsert, std::unique_ptr<joda::ui::gui::Command> command, const settings::PipelineStep *pipelineStepBefore);
  void erasePipelineStep(const Command *, bool updateHistory = true);
  void setActive(bool setActive) override;
  const joda::settings::Pipeline &getPipeline()
  {
    return mSettings;
  }

  joda::settings::Pipeline &mutablePipeline()
  {
    return mSettings;
  }

  QWidget *getEditPanel() override
  {
    return this;
  }

  nlohmann::json toJson(const std::string &titlePrefix) override
  {
    return {};
  }

  void toSettings() override;
  void fromSettings(const joda::settings::Pipeline &settings);
  void clearPipeline();
  void pipelineSavedEvent();
  void openPipelineSettings();

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  void previewThread();

  // ACTIONS///////////////////////////////////////////////////
  QAction *mUndoAction;
  QAction *mHistoryAction;
  QAction *mActionDisabled;

  /////////////////////////////////////////////////////
  QToolBar *mToolbar;
  QVBoxLayout *mLayout;

  /////////////////////////////////////////////////////
  DialogHistory *mDialogHistory;
  DialogImageViewer *mPreviewImage            = nullptr;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;
  bool mPreviewInProgress                     = false;
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
  joda::ctrl::Preview::PreviewResults mPreviewResults;
  DialogPreviewResults *mPreviewResultsDialog;

  struct PreviewJob
  {
    settings::AnalyzeSettings settings;
    joda::ctrl::Controller *controller;
    DialogImageViewer *previewPanel;
    std::tuple<std::filesystem::path, int32_t, joda::ome::OmeInfo> selectedImage;
    int32_t pipelinePos;
    int32_t selectedTileX = 0;
    int32_t selectedTileY = 0;
    int32_t timeStack     = 0;
    std::map<enums::ClassIdIn, QString> classes;
    settings::ObjectInputClassesExp classesToHide;
    joda::thread::ThreadingSettings threadSettings;
  };

  bool mStopped = false;
  joda::TSQueue<PreviewJob> mPreviewQue;
  std::mutex mCheckForEmptyMutex;
  std::mutex mShutingDownMutex;

private slots:
  /////////////////////////////////////////////////////
  void updatePreview();
  void onPreviewStarted();
  void onPreviewFinished();
  void valueChangedEvent();
  void metaChangedEvent();
  void closeWindow();
  void onClassificationNameChanged();
};

}    // namespace joda::ui::gui
