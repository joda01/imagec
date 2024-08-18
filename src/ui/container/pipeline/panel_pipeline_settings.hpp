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
#include "backend/settings/pipeline/pipeline.hpp"
#include "controller/controller.hpp"
#include "ui/container/container_base.hpp"
#include "ui/container/pipeline/panel_channel_overview.hpp"
#include "ui/container/setting/setting.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui::qt {

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

  void toSettings() override
  {
  }

  void fromSettings(const joda::settings::Pipeline &settings)
  {
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  void createSettings(WindowMain *windowMain);

  /////////////////////////////////////////////////////
  helper::LayoutGenerator mLayout;
  std::shared_ptr<Setting<QString, QString>> mChannelName;
  std::shared_ptr<Setting<QString, int32_t>> mColorAndChannelIndex;

  /////////////////////////////////////////////////////
  PanelPreview *mPreviewImage = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter                         = 0;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;
  WindowMain *mWindowMain;

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
};

}    // namespace joda::ui::qt
