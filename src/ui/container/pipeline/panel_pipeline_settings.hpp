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

#include <qtmetamacros.h>
#include <QtWidgets>
#include <memory>
#include <mutex>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "controller/controller.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/container_base.hpp"
#include "ui/container/pipeline/panel_channel_overview.hpp"
#include "ui/container/setting/setting.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui {

class WindowMain;

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

  void addPipelineStep(std::shared_ptr<joda::ui::Command> command);

  void setActive(bool setActive) override
  {
    if(!mIsActiveShown && setActive) {
      mIsActiveShown = true;
      updatePreview(-1, -1);
    }
    if(!setActive) {
      mIsActiveShown = false;
      std::lock_guard<std::mutex> lock(mPreviewMutex);
      mPreviewCounter = 0;
      mPreviewImage->resetImage("");
    }
  }

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

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  void createSettings(WindowMain *windowMain);

  /////////////////////////////////////////////////////
  helper::LayoutGenerator mLayout;
  std::shared_ptr<Setting<std::string, std::string>> mPipelineName;
  std::shared_ptr<Setting<std::string, int32_t>> mCStackIndex;
  std::shared_ptr<Setting<enums::ZProjection, int32_t>> mZProjection;
  std::shared_ptr<Setting<enums::ClusterIdIn, int32_t>> mDefaultClusterId;

  /////////////////////////////////////////////////////
  PanelPreview *mPreviewImage = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter                         = 0;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;
  WindowMain *mWindowMain;

  // PIPELINE STEPS //////////////////////////////////////////////////
  QVBoxLayout *mPipelineSteps;
  std::vector<std::shared_ptr<Command>> mCommands;

  /////////////////////////////////////////////////////
  int32_t mSelectedTileX = 0;
  int32_t mSelectedTileY = 0;

  joda::ctrl::Preview mPreviewObject;
  PanelChannelOverview *mOverview;
  joda::settings::Pipeline &mSettings;

private slots:
  /////////////////////////////////////////////////////
  void updatePreview(int32_t newImgIdex, int32_t selectedSeries);
  void onTileClicked(int32_t tileX, int32_t tileY);
  void onPreviewStarted();
  void onPreviewFinished();
  void valueChangedEvent();
  void metaChangedEvent();
};

}    // namespace joda::ui
