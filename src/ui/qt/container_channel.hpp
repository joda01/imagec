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
#include <memory>
#include "ui/qt/container_function.hpp"
#include "ui/qt/panel_channel_edit.hpp"
#include "ui/qt/panel_channel_overview.hpp"

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
  /////////////////////////////////////////////////////
  ContainerChannel(WindowMain *windowMain);
  PanelChannelOverview *getOverviewPanel()
  {
    return mPanelOverview;
  }
  PanelChannelEdit *getEditPanel()
  {
    return mPanelEdit;
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

  /////////////////////////////////////////////////////
  PanelChannelOverview *mPanelOverview;
  PanelChannelEdit *mPanelEdit;
};

}    // namespace joda::ui::qt
