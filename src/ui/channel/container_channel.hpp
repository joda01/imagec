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
#include "backend/settings/analze_settings_parser.hpp"
#include "backend/settings/channel_settings.hpp"
#include "backend/settings/pipeline_settings.hpp"
#include "ui/container_base.hpp"
#include "ui/container_function.hpp"
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
  ContainerChannel(const ContainerChannel &)            = default;
  ContainerChannel(ContainerChannel &&)                 = delete;
  ContainerChannel &operator=(const ContainerChannel &) = default;
  ContainerChannel &operator=(ContainerChannel &&)      = delete;
  /////////////////////////////////////////////////////
  ContainerChannel(WindowMain *windowMain);
  ~ContainerChannel();
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

  void fromJson(std::optional<joda::settings::json::ChannelSettings>,
                std::optional<joda::settings::json::PipelineStepVoronoi>) override;
  [[nodiscard]] ConvertedChannels toJson() const override;

  std::tuple<std::string, float> getMinColocFactor() override
  {
    if(mColocGroup->hasValue()) {
      return {mColocGroup->getValue().toStdString(), mColocGroup->getValueSecond() / 100.0F};
    }
    return {"NONE", 0};
  }

  void setMinColocFactor(const std::string &group, float newValue) override
  {
    if(group == mColocGroup->getValue().toStdString()) {
      mColocGroup->setValueSecond(newValue * 100.0F);
    }
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<ContainerFunction<QString, QString>> mChannelName;
  std::shared_ptr<ContainerFunction<QString, QString>> mChannelType;
  std::shared_ptr<ContainerFunction<QString, int>> mColorAndChannelIndex;

  std::shared_ptr<ContainerFunction<QString, QString>> mThresholdAlgorithm;
  std::shared_ptr<ContainerFunction<int, int>> mThresholdValueMin;
  std::shared_ptr<ContainerFunction<float, float>> mMinCircularity;
  std::shared_ptr<ContainerFunction<int, int>> mMinParticleSize;
  std::shared_ptr<ContainerFunction<int, int>> mMaxParticleSize;
  std::shared_ptr<ContainerFunction<int, int>> mSnapAreaSize;

  std::shared_ptr<ContainerFunction<QString, QString>> mZProjection;
  std::shared_ptr<ContainerFunction<int, int>> mMarginCrop;
  std::shared_ptr<ContainerFunction<int, int>> mSubtractChannel;
  std::shared_ptr<ContainerFunction<bool, bool>> mMedianBackgroundSubtraction;
  std::shared_ptr<ContainerFunction<int, QString>> mRollingBall;
  std::shared_ptr<ContainerFunction<int, int>> mGaussianBlur;
  std::shared_ptr<ContainerFunction<int, int>> mSmoothing;
  std::shared_ptr<ContainerFunction<QString, QString>> mEdgeDetection;
  std::shared_ptr<ContainerFunction<int, int>> mTetraspeckRemoval;

  std::shared_ptr<ContainerFunction<QString, QString>> mUsedDetectionMode;
  std::shared_ptr<ContainerFunction<float, float>> mMinProbability;
  std::shared_ptr<ContainerFunction<QString, QString>> mAIModels;

  // Cross-Channel//////////////////////////////////
  std::shared_ptr<ContainerFunction<QString, int>> mColocGroup;
  std::shared_ptr<ContainerFunction<QString, int>> mCrossChannelIntensity;
  std::shared_ptr<ContainerFunction<QString, int>> mCrossChannelCount;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PanelChannelOverview *mPanelOverview;
  PanelChannelEdit *mPanelEdit;
};

}    // namespace joda::ui::qt
