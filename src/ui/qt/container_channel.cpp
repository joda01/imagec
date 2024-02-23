///
/// \file      container_channel.cpp
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

#include "container_channel.hpp"
#include <qwidget.h>
#include <cstdint>
#include <memory>
#include <string>
#include "ui/qt/panel_channel_overview.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerChannel::ContainerChannel(WindowMain *windowMain)
{
  mChannelName = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-text-50.png", "Name", "Channel Name", ""));

  mChannelIndex = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-layers-50.png", "Index", "Channel index", "", 0,
                                 {{0, "Channel 1"}, {1, "Channel 2"}, {2, "Channel 3"}, {3, "Channel 4"}}));

  mChannelType = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-unknown-status-50.png", "Type", "Channel type", "", "SPOT",
                                     {{"SPOT", "Spot"},
                                      {"SPOT_REFERENCE", "Reference Spot"},
                                      {"NUCLEUS", "Nucleus"},
                                      {"CELL", "Cell"},
                                      {"BACKGROUND", "Background"}}));

  mThresholdAlgorithm = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-lambda-50.png", "Threshold", "Threshold algorithm", "", "MANUAL",
                                     {{"MANUAL", "Manual"},
                                      {"LI", "Li"},
                                      {"MIN_ERROR", "Min. error"},
                                      {"TRIANGLE", "Triangle"},
                                      {"MOMENTS", "Moments"}}));

  mThresholdValueMin = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-grayscale-50.png", "[0 - 65535]", "Min. threshold", "px", 1000, 0, 65535));

  mMinCircularity = std::shared_ptr<ContainerFunction<float>>(
      new ContainerFunction<float>("icons8-ellipse-50.png", "[0 - 1]", "Min. circularity", "%", 0.8, 0, 1));

  mMinParticleSize = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]",
                                 "Min. particle size", "px", 1, 0, INT32_MAX));

  mSnapAreaSize = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-initial-state-50.png", "[0 - 65535]", "Snap area size", "px", 0, 0, 65535));

  mZProjection = std::shared_ptr<ContainerFunction<QString>>(new ContainerFunction<QString>(
      "icons8-layers-50.png", "Z-Projection", "Z-Projection", "", "OFF",
      {{"OFF", "Off"}, {"MAX_INTENSITY", "Max. intensity"}, {"PROJECTION_3D", "3D projection"}}));

  mMarginCrop      = std::shared_ptr<ContainerFunction<int>>(new ContainerFunction<int>(
      "icons8-crop-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Crop margin", "px", 0, 0, INT32_MAX));
  mSubtractChannel = std::shared_ptr<ContainerFunction<int>>(new ContainerFunction<int>(
      "icons8-layers-50.png", "Index", "Subtract other channel", "", -1,
      {{-1, "Off"}, {0, "Channel 1"}, {1, "Channel 2"}, {2, "Channel 3"}, {3, "Channel 4"}}));

  mMedianBackgroundSubtraction = std::shared_ptr<ContainerFunction<bool>>(
      new ContainerFunction<bool>("icons8-baseline-50.png", "On/Off", "Median background subtraction", false));
  mRollingBall = std::shared_ptr<ContainerFunction<int>>(new ContainerFunction<int>(
      "icons8-bubble-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Rolling ball", "px", 1, 0, INT32_MAX));

  mGaussianBlur = std::shared_ptr<ContainerFunction<int>>(new ContainerFunction<int>(
      "icons8-blur-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Gaussian blur", "px", 1, 0, INT32_MAX));

  mSmoothing = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-cleanup-noise-50.png", "Kernel size", "Smoothing", "", -1,
                                 {{-1, "Off"}, {3, "3x3"}, {5, "5x5"}, {7, "7x7"}}));
  mEdgeDetection = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-triangle-50.png", "Threshold", "Threshold algorithm", "", "NONE",
                                     {{"NONE", "Off"}, {"SOBEL", "Sobel"}, {"CANNY", "Canny"}}));
  mTetraspeckRemoval = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-final-state-50.png", "Index", "Tetraspeck removal", "", 0,
                                 {{0, "Channel 1"}, {1, "Channel 2"}, {2, "Channel 3"}, {3, "Channel 4"}}));

  //
  // Create panels -> Must be after creating the functions
  //
  mPanelOverview = std::shared_ptr<PanelChannelOverview>(new PanelChannelOverview(windowMain, this));
  mPanelEdit     = std::shared_ptr<PanelChannelEdit>(new PanelChannelEdit(windowMain, this));
}

}    // namespace joda::ui::qt
