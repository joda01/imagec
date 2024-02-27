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
#include <optional>
#include <string>
#include "backend/settings/channel_settings.hpp"
#include "ui/qt/panel_channel_overview.hpp"
#include "window_main.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerChannel::ContainerChannel(WindowMain *windowMain) : mWindowMain(windowMain)
{
  mChannelName = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-text-50.png", "Name", "Channel Name", "Name"));

  mChannelIndex = std::shared_ptr<ContainerFunction<int>>(new ContainerFunction<int>("icons8-layers-50.png", "Index",
                                                                                     "Channel index", "", 0,
                                                                                     {{0, "Channel 0"},
                                                                                      {1, "Channel 1"},
                                                                                      {2, "Channel 2"},
                                                                                      {3, "Channel 3"},
                                                                                      {4, "Channel 4"},
                                                                                      {5, "Channel 5"},
                                                                                      {6, "Channel 6"},
                                                                                      {7, "Channel 7"},
                                                                                      {8, "Channel 8"},
                                                                                      {9, "Channel 9"},
                                                                                      {10, "Channel 10"},
                                                                                      {11, "Channel 11"},
                                                                                      {12, "Channel 12"}}));

  mChannelType = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-unknown-status-50.png", "Type", "Channel type", "", "SPOT",
                                     {{"SPOT", "Spot"},
                                      {"SPOT_REFERENCE", "Reference Spot"},
                                      {"NUCLEUS", "Nucleus"},
                                      {"CELL", "Cell"},
                                      {"BACKGROUND", "Background"}}));

  mUsedDetectionMode = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-mesh-50.png", "Threshold", "Detection mode", "", "THRESHOLD",
                                     {{"THRESHOLD", "Threshold"}, {"AI", "Artificial intelligence"}}));

  mThresholdAlgorithm = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-ksi-50.png", "Threshold", "Threshold algorithm", "", "MANUAL",
                                     {{"MANUAL", "Manual"},
                                      {"LI", "Li"},
                                      {"MIN_ERROR", "Min. error"},
                                      {"TRIANGLE", "Triangle"},
                                      {"MOMENTS", "Moments"}}));

  mThresholdValueMin = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-grayscale-50.png", "[0 - 65535]", "Min. threshold", "px", 1000, 0, 65535));

  mMinProbability = std::shared_ptr<ContainerFunction<float>>(
      new ContainerFunction<float>("icons8-percentage-50.png", "[0 - 1]", "Min. probability", "%", 0.5, 0, 1));

  mAIModels = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-mind-map-50.png", "AI model", "AI model", "", "OFF", {{"OFF", "Off"}}));

  mMinCircularity = std::shared_ptr<ContainerFunction<float>>(
      new ContainerFunction<float>("icons8-ellipse-50.png", "[0 - 1]", "Min. circularity", "%", 0.1, 0, 1));

  mMinParticleSize = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]",
                                 "Min. particle size", "px", 1, 0, INT32_MAX));
  mMaxParticleSize = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]",
                                 "Max. particle size", "px", 1, 0, INT32_MAX));

  mSnapAreaSize = std::shared_ptr<ContainerFunction<int>>(new ContainerFunction<int>(
      "icons8-initial-state-50.png", "[0 - 65535]", "Snap area size", "px", std::nullopt, 0, 65535));

  mZProjection = std::shared_ptr<ContainerFunction<QString>>(new ContainerFunction<QString>(
      "icons8-layers-50.png", "Z-Projection", "Z-Projection", "", "OFF",
      {{"OFF", "Off"}, {"MAX_INTENSITY", "Max. intensity"}, {"PROJECTION_3D", "3D projection"}}));

  mMarginCrop = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-crop-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Crop margin", "px",
                                 std::nullopt, 0, INT32_MAX));
  mSubtractChannel = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-layers-50.png", "Index", "Subtract other channel", "", -1,
                                 {{-1, "Off"},
                                  {0, "Channel 0"},
                                  {1, "Channel 1"},
                                  {2, "Channel 2"},
                                  {3, "Channel 3"},
                                  {4, "Channel 4"},
                                  {5, "Channel 5"},
                                  {6, "Channel 6"},
                                  {7, "Channel 7"},
                                  {8, "Channel 8"},
                                  {9, "Channel 9"},
                                  {10, "Channel 10"},
                                  {11, "Channel 11"},
                                  {12, "Channel 12"}}));

  mMedianBackgroundSubtraction = std::shared_ptr<ContainerFunction<bool>>(
      new ContainerFunction<bool>("icons8-baseline-50.png", "On/Off", "Median background subtraction", false));
  mRollingBall = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-bubble-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Rolling ball",
                                 "px", std::nullopt, 0, INT32_MAX));

  mGaussianBlur = std::shared_ptr<ContainerFunction<int>>(new ContainerFunction<int>(
      "icons8-blur-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Gaussian blur", "px", -1,
      {{-1, "Off"}, {3, "3x3"}, {5, "5x5"}, {7, "7x7"}}, {{1, "1x"}, {2, "2x"}, {3, "3x"}}));

  mSmoothing = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-cleanup-noise-50.png", "Kernel size", "Smoothing", "", -1,
                                 {{-1, "Off"}, {1, "x1"}, {2, "x2"}, {3, "x3"}}));
  mEdgeDetection     = std::shared_ptr<ContainerFunction<QString>>(new ContainerFunction<QString>(
      "icons8-triangle-50.png", "Threshold", "Edge detection", "", "NONE",
      {{"NONE", "Off"}, {"SOBEL", "Sobel"}, {"CANNY", "Canny"}}, {{"XY", "xy"}, {"X", "x"}, {"Y", "y"}}));
  mTetraspeckRemoval = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-final-state-50.png", "Index", "Tetraspeck removal", "", -1,
                                 {{0, "Channel 0"},
                                  {1, "Channel 1"},
                                  {2, "Channel 2"},
                                  {3, "Channel 3"},
                                  {4, "Channel 4"},
                                  {5, "Channel 5"},
                                  {6, "Channel 6"},
                                  {7, "Channel 7"},
                                  {8, "Channel 8"},
                                  {9, "Channel 9"},
                                  {10, "Channel 10"},
                                  {11, "Channel 11"},
                                  {12, "Channel 12"}}));

  //
  // Cell approximation
  //
  mEnableCellApproximation = std::shared_ptr<ContainerFunction<bool>>(
      new ContainerFunction<bool>("dom-voronoi-50.png", "On/Off", "Enable cell approximation.", false));
  mMaxCellRadius = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]",
                                 "Max. cell radius", "px", 100, 0, INT32_MAX));

  //
  // Create panels -> Must be after creating the functions
  //
  mPanelOverview = new PanelChannelOverview(windowMain, this);
  mPanelEdit     = new PanelChannelEdit(windowMain, this);
}

///
/// \brief      Load values
/// \author     Joachim Danmayr
///
void ContainerChannel::fromJson(const joda::settings::json::ChannelSettings &chSettings,
                                std::optional<joda::settings::json::PipelineStepCellApproximation> cellApprox)
{
}

///
/// \brief      Get values
/// \author     Joachim Danmayr
///
ContainerChannel::ConvertedChannels ContainerChannel::toJson() const
{
  nlohmann::json chSettings;

  chSettings["info"]["index"]  = mChannelIndex->getValue();
  chSettings["info"]["series"] = mWindowMain->getSelectedSeries();
  chSettings["info"]["type"]   = mChannelType->getValue().toStdString();
  chSettings["info"]["label"]  = "";
  chSettings["info"]["name"]   = mChannelName->getValue().toStdString();

  // Preprocessing
  nlohmann::json jsonArray = nlohmann::json::array();    // Initialize an empty JSON array
  jsonArray.push_back({{"z_stack", {{"value", mZProjection->getValue().toStdString()}}}});
  if(mMarginCrop->hasValue()) {
    jsonArray.push_back({{"margin_crop", {{"value", static_cast<int>(mMarginCrop->getValue())}}}});
  }
  if(mEdgeDetection->getValue() != "NONE") {
    jsonArray.push_back({{"edge_detection",
                          {{"value", mEdgeDetection->getValue().toStdString()},
                           {"direction", mEdgeDetection->getValueSecond().toStdString()}}}});
  }

  if(mRollingBall->hasValue()) {
    jsonArray.push_back({{"rolling_ball", {{"value", static_cast<int>(mRollingBall->getValue())}}}});
  }

  if(mGaussianBlur->hasValue()) {
    jsonArray.push_back(
        {{"gaussian_blur", {{"kernel_size", mGaussianBlur->getValue()}, {"repeat", mGaussianBlur->getValue()}}}});
  }
  if(mSmoothing->hasValue()) {
    jsonArray.push_back({{"smoothing", {{"repeat", mSmoothing->getValue()}}}});
  }
  if(mMedianBackgroundSubtraction->hasValue()) {
    jsonArray.push_back({{"median_bg_subtraction", {{"kernel_size", 3}}}});
  }
  if(mSubtractChannel->hasValue()) {
    jsonArray.push_back({{"subtract_channel", {{"channel_index", mSubtractChannel->getValue()}}}});
  }

  chSettings["preprocessing"] = jsonArray;

  // Detections
  chSettings["detection"]["mode"] = mUsedDetectionMode->getValue().toStdString();

  chSettings["detection"]["threshold"]["threshold_algorithm"] = mThresholdAlgorithm->getValue().toStdString();
  chSettings["detection"]["threshold"]["threshold_min"]       = static_cast<int>(mThresholdValueMin->getValue());
  chSettings["detection"]["threshold"]["threshold_max"]       = UINT16_MAX;

  chSettings["detection"]["ai"]["model_name"]      = mAIModels->getValue().toStdString();
  chSettings["detection"]["ai"]["probability_min"] = mMinProbability->getValue();

  // Filtering
  if(mMinParticleSize->hasValue()) {
    chSettings["filter"]["min_particle_size"] = mMinParticleSize->getValue();
  } else {
    chSettings["filter"]["min_particle_size"] = 0;
  }

  if(mMaxParticleSize->hasValue()) {
    chSettings["filter"]["max_particle_size"] = mMaxParticleSize->getValue();
  } else {
    chSettings["filter"]["max_particle_size"] = INT32_MAX;
  }

  chSettings["filter"]["min_circularity"]              = mMinCircularity->getValue();
  chSettings["filter"]["snap_area_size"]               = mSnapAreaSize->getValue();
  chSettings["filter"]["reference_spot_channel_index"] = mTetraspeckRemoval->getValue();

  return {.channelSettings = chSettings, .pipelineStep = {}};
}

}    // namespace joda::ui::qt
