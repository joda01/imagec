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
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/thread_safe_queue.hpp"
#include "backend/settings/analze_settings.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/container_base.hpp"
#include "ui/container/pipeline/panel_channel_overview.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui {

class WindowMain;
class AddCommandButtonBase;
class PanelClassification;

class PanelPipelineSettings : public QWidget, public ContainerBase
{
  Q_OBJECT

  friend class PanelChannelOverview;

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();

public:
  PanelPipelineSettings(WindowMain *wm, joda::settings::Pipeline &settings);
  ~PanelPipelineSettings();

  void addPipelineStep(std::unique_ptr<joda::ui::Command> command, const settings::PipelineStep *);
  void insertNewPipelineStep(int32_t posToInsert, std::unique_ptr<joda::ui::Command> command, const settings::PipelineStep *pipelineStepBefore);
  void erasePipelineStep(const Command *);
  void setActive(bool setActive) override;
  const joda::settings::Pipeline &getPipeline()
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

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  void createSettings(helper::TabWidget *, WindowMain *windowMain);
  void openTemplate();
  void saveAsTemplate();
  void previewThread();
  void copyPipeline();

  /////////////////////////////////////////////////////
  helper::LayoutGenerator mLayout;
  std::unique_ptr<SettingLineEdit<std::string>> pipelineName;
  std::unique_ptr<SettingComboBox<int32_t>> cStackIndex;
  std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjection;
  std::unique_ptr<SettingComboBox<enums::ClusterId>> defaultClusterId;
  std::unique_ptr<SettingComboBox<enums::ClassId>> defaultClassId;

  /////////////////////////////////////////////////////
  PanelPreview *mPreviewImage                 = nullptr;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;
  bool mPreviewInProgress                     = false;
  WindowMain *mWindowMain;
  AddCommandButtonBase *mTopAddCommandButton;

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
    std::tuple<std::filesystem::path, uint32_t, joda::ome::OmeInfo> selectedImage;
    int32_t pipelinePos;
    int32_t selectedTileX = 0;
    int32_t selectedTileY = 0;
    std::tuple<std::map<enums::ClusterIdIn, QString>, std::map<enums::ClassIdIn, QString>> clustersAndClasses;
    settings::ObjectInputClusters clustersClassesToShow;
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
};

}    // namespace joda::ui
