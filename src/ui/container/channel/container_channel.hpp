///
/// \file      container_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A container which holds all functions of a channel
///            The channel container has two possible views:
///              The overview view and the edit view
///

#pragma once

#include <QtWidgets>
#include <map>
#include <memory>
#include <optional>
#include "../container_base.hpp"
#include "../container_function.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"
#include "backend/settings/detection/detection_settings.hpp"
#include "backend/settings/preprocessing/functions/edge_detection.hpp"
#include "backend/settings/preprocessing/functions/rolling_ball.hpp"
#include "backend/settings/preprocessing/functions/zstack.hpp"
#include <nlohmann/json_fwd.hpp>
#include "panel_channel_edit.hpp"
#include "panel_channel_overview.hpp"

namespace joda::ui::qt {

///
/// \class      ContainerChannel
/// \author     Joachim Danmayr
/// \brief
///
class ContainerChannel : public ContainerBase
{
  friend class PanelChannelOverview;
  friend class PanelChannelEdit;

public:
  ContainerChannel(const ContainerChannel &)            = delete;
  ContainerChannel(ContainerChannel &&)                 = delete;
  ContainerChannel &operator=(const ContainerChannel &) = delete;
  ContainerChannel &operator=(ContainerChannel &&)      = delete;

  /////////////////////////////////////////////////////
  ContainerChannel(WindowMain *windowMain, joda::settings::ChannelSettings &settings);
  ~ContainerChannel() override;
  PanelChannelOverview *getOverviewPanel() override
  {
    return mPanelOverview;
  }
  PanelChannelEdit *getEditPanel() override
  {
    return mPanelEdit;
  }
  void setActive(bool setActive) override
  {
    if(mPanelEdit != nullptr) {
      mPanelEdit->setActive(setActive);
    }
  }

  void fromSettings() override;
  void toSettings() override;

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<ContainerFunction<QString, QString>> mChannelName;
  std::shared_ptr<ContainerFunction<joda::settings::ChannelSettingsMeta::Type, QString>> mChannelType;
  std::shared_ptr<ContainerFunction<QString, joda::settings::ChannelIndex>> mColorAndChannelIndex;

  std::shared_ptr<ContainerFunction<joda::settings::ThresholdSettings::Mode, QString>> mThresholdAlgorithm;
  std::shared_ptr<ContainerFunction<int, int>> mThresholdValueMin;
  std::shared_ptr<ContainerFunction<bool, bool>> mWateredSegmentation;

  // Filtering
  std::shared_ptr<ContainerFunction<float, float>> mMinCircularity;
  std::shared_ptr<ContainerFunction<int, int>> mMinParticleSize;
  std::shared_ptr<ContainerFunction<int, int>> mMaxParticleSize;
  std::shared_ptr<ContainerFunction<int, int>> mSnapAreaSize;

  std::shared_ptr<ContainerFunction<joda::settings::ZStackProcessing::ZStackMethod, QString>> mZProjection;
  std::shared_ptr<ContainerFunction<int, int>> mMarginCrop;
  std::shared_ptr<ContainerFunction<joda::settings::ChannelIndex, int>> mSubtractChannel;
  std::shared_ptr<ContainerFunction<int, int>> mMedianBackgroundSubtraction;
  std::shared_ptr<ContainerFunction<int, joda::settings::RollingBall::BallType>> mRollingBall;
  std::shared_ptr<ContainerFunction<int, int>> mGaussianBlur;
  std::shared_ptr<ContainerFunction<int, int>> mSmoothing;
  std::shared_ptr<ContainerFunction<joda::settings::EdgeDetection::Mode, joda::settings::EdgeDetection::Direction>>
      mEdgeDetection;
  std::shared_ptr<ContainerFunction<joda::settings::ChannelIndex, int>> mTetraspeckRemoval;

  std::shared_ptr<ContainerFunction<joda::settings::DetectionSettings::DetectionMode, QString>> mUsedDetectionMode;
  std::shared_ptr<ContainerFunction<float, float>> mMinProbability;
  std::shared_ptr<ContainerFunction<QString, QString>> mAIModels;

  // Cross-Channel//////////////////////////////////
  std::shared_ptr<ContainerFunction<QString, int>> mCrossChannelIntensity;
  std::shared_ptr<ContainerFunction<QString, int>> mCrossChannelCount;

  // Reporting//////////////////////////////////
  settings::ChannelSettings &mSettings;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PanelChannelOverview *mPanelOverview;
  PanelChannelEdit *mPanelEdit;
};

}    // namespace joda::ui::qt
