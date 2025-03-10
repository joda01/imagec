///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

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
#include "ui/gui/container/pipeline/panel_channel_overview.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/container/setting/setting_spinbox.hpp"
#include "ui/gui/container/setting/setting_text_edit.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/panel_preview.hpp"

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

class PanelPipelineSettings : public QWidget, public ContainerBase
{
  Q_OBJECT

  friend class PanelChannelOverview;

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();

public:
  PanelPipelineSettings(WindowMain *wm, joda::settings::Pipeline &settings, std::shared_ptr<DialogCommandSelection> &commandSelectionDialog);
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

  QWidget *getOverviewPanel() override
  {
    return mOverview;
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

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  void createSettings(helper::TabWidget *, WindowMain *windowMain);
  void openTemplate();
  void saveAsTemplate();
  void previewThread();
  void copyPipeline();

  // ACTIONS///////////////////////////////////////////////////
  QAction *mHistoryAction;
  QAction *mActionDisabled;

  /////////////////////////////////////////////////////
  helper::LayoutGenerator mLayout;
  std::unique_ptr<SettingLineEdit<std::string>> pipelineName;
  std::unique_ptr<SettingTextEdit> pipelineNotes;
  std::unique_ptr<SettingComboBox<int32_t>> cStackIndex;
  std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjection;
  std::unique_ptr<SettingSpinBox<int32_t>> zStackIndex;
  std::unique_ptr<SettingComboBoxClassesOutN> defaultClassId;

  /////////////////////////////////////////////////////
  DialogHistory *mDialogHistory;
  PanelPreview *mPreviewImage                 = nullptr;
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
  int32_t mSelectedTileX           = 0;
  int32_t mSelectedTileY           = 0;

  // joda::ctrl::Preview mPreviewObject;
  PanelChannelOverview *mOverview;
  joda::settings::Pipeline &mSettings;

  struct PreviewJob
  {
    settings::AnalyzeSettings settings;
    joda::ctrl::Controller *controller;
    PanelPreview *previewPanel;
    std::tuple<std::filesystem::path, int32_t, joda::ome::OmeInfo> selectedImage;
    int32_t pipelinePos;
    int32_t selectedTileX = 0;
    int32_t selectedTileY = 0;
    std::map<enums::ClassIdIn, QString> classes;
    settings::ObjectInputClasses classesToShow;
    joda::thread::ThreadingSettings threadSettings;
  };

  bool mStopped = false;
  joda::TSQueue<PreviewJob> mPreviewQue;
  std::mutex mCheckForEmptyMutex;
  std::mutex mShutingDownMutex;

private slots:
  /////////////////////////////////////////////////////
  void updatePreview();
  void onTileClicked(int32_t tileX, int32_t tileY);
  void onPreviewStarted();
  void onPreviewFinished();
  void valueChangedEvent();
  void metaChangedEvent();
  void closeWindow();
  void deletePipeline();
  void onClassificationNameChanged();
  void onZProjectionChanged();
};

}    // namespace joda::ui::gui
