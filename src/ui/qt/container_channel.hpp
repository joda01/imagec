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
#include "ui/qt/container_function.hpp"
#include "ui/qt/panel_channel_edit.hpp"
#include "ui/qt/panel_channel_overview.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

///
/// \class      ContainerChannel
/// \author     Joachim Danmayr
/// \brief
///
class ContainerChannel : std::enable_shared_from_this<ContainerChannel>
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
  PanelChannelOverview *getOverviewPanel()
  {
    return mPanelOverview;
  }
  PanelChannelEdit *getEditPanel()
  {
    return mPanelEdit;
  }

  using IntersectionGroup = int;
  struct IntersectionChannel
  {
    std::set<int> channel = {};
    float minIntersect    = 0;
  };
  using IntersectionSettings = std::map<IntersectionGroup, IntersectionChannel>;

  struct IntersectionRead
  {
    int32_t intersectionGroup = -1;
    float minColocFactor      = 0;
  };

  struct ConvertedChannels
  {
    nlohmann::json channelSettings;
    nlohmann::json pipelineStep;
    IntersectionSettings intersection;
  };

  void fromJson(const joda::settings::json::ChannelSettings &,
                std::optional<joda::settings::json::PipelineStepCellApproximation>,
                std::optional<IntersectionRead> channelIntersection,
                std::optional<IntersectionRead> cellApproxIntersection);
  ConvertedChannels toJson() const;

  std::tuple<int32_t, float> getMinColocFactor()
  {
    if(mColocGroup->hasValue()) {
      return {mColocGroup->getValue(), mColocGroup->getValueSecond() / 100.0F};
    }
    return {-1, 0};
  }

  void setMinColocFactor(int group, float newValue)
  {
    if(group == mColocGroup->getValue()) {
      mColocGroup->setValueSecond(newValue * 100.0F);
    }
  }

  void stopPreviewGeneration()
  {
    if(mPanelEdit != nullptr) {
      mPanelEdit->stopPreviewGeneration();
    }
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<ContainerFunction<QString>> mChannelName;
  std::shared_ptr<ContainerFunction<int>> mChannelIndex;
  std::shared_ptr<ContainerFunction<QString>> mChannelType;
  std::shared_ptr<ContainerFunction<QString>> mThresholdAlgorithm;
  std::shared_ptr<ContainerFunction<int>> mThresholdValueMin;
  std::shared_ptr<ContainerFunction<float>> mMinCircularity;
  std::shared_ptr<ContainerFunction<int>> mMinParticleSize;
  std::shared_ptr<ContainerFunction<int>> mMaxParticleSize;
  std::shared_ptr<ContainerFunction<int>> mSnapAreaSize;

  std::shared_ptr<ContainerFunction<QString>> mZProjection;
  std::shared_ptr<ContainerFunction<int>> mMarginCrop;
  std::shared_ptr<ContainerFunction<int>> mSubtractChannel;
  std::shared_ptr<ContainerFunction<bool>> mMedianBackgroundSubtraction;
  std::shared_ptr<ContainerFunction<int>> mRollingBall;
  std::shared_ptr<ContainerFunction<int>> mGaussianBlur;
  std::shared_ptr<ContainerFunction<int>> mSmoothing;
  std::shared_ptr<ContainerFunction<QString>> mEdgeDetection;
  std::shared_ptr<ContainerFunction<int>> mTetraspeckRemoval;

  std::shared_ptr<ContainerFunction<QString>> mUsedDetectionMode;
  std::shared_ptr<ContainerFunction<float>> mMinProbability;
  std::shared_ptr<ContainerFunction<QString>> mAIModels;

  // Cell approximation//////////////////////////////////
  std::shared_ptr<ContainerFunction<bool>> mEnableCellApproximation;
  std::shared_ptr<ContainerFunction<int>> mMaxCellRadius;
  std::shared_ptr<ContainerFunction<int>> mColocGroupCellApproximation;

  // Colocalization//////////////////////////////////
  std::shared_ptr<ContainerFunction<int>> mColocGroup;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PanelChannelOverview *mPanelOverview;
  PanelChannelEdit *mPanelEdit;
};

}    // namespace joda::ui::qt
