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
#include <vector>
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/settings/channel_settings.hpp"
#include "ui/container_function.hpp"
#include "ui/window_main.hpp"
#include "panel_channel_overview.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerChannel::ContainerChannel(WindowMain *windowMain) : mWindowMain(windowMain)
{
  mChannelName = std::shared_ptr<ContainerFunction<QString, QString>>(
      new ContainerFunction<QString, QString>("icons8-text-50.png", "Name", "Channel Name", "Name"));

  mChannelType = std::shared_ptr<ContainerFunction<QString, QString>>(
      new ContainerFunction<QString, QString>("icons8-unknown-status-50.png", "Type", "Channel type", "", "SPOT",
                                              {{"SPOT", "Spot"},
                                               {"SPOT_REFERENCE", "Reference Spot"},
                                               {"NUCLEUS", "Nucleus"},
                                               {"CELL", "Cell"},
                                               {"BACKGROUND", "Background"}}));

  mColorAndChannelIndex = std::shared_ptr<ContainerFunction<QString, int>>(
      new ContainerFunction<QString, int>("icons8-unknown-status-50.png", "Type", "Channel index", "", "#B91717",
                                          {{"#B91717", "", "icons8-bubble-50red-#B91717.png"},
                                           {"#06880C", "", "icons8-bubble-50 -green-#06880C.png"},
                                           {"#1771B9", "", "icons8-bubble-blue-#1771B9-50.png"},
                                           {"#FBEA25", "", "icons8-bubble-50-yellow-#FBEA25.png"},
                                           {"#6F03A6", "", "icons8-bubble-50-violet-#6F03A6.png"},
                                           {"#818181", "", "icons8-bubble-50-gray-#818181.png"},
                                           /*{"#000000", "", "icons8-bubble-50-black-#000000.png"}*/},
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
                                           {12, "Channel 12"}},
                                          0));

  mUsedDetectionMode = std::shared_ptr<ContainerFunction<QString, QString>>(
      new ContainerFunction<QString, QString>("icons8-mesh-50.png", "Threshold", "Detection mode", "", "THRESHOLD",
                                              {{"THRESHOLD", "Threshold"}, {"AI", "Artificial intelligence"}}));

  mThresholdAlgorithm = std::shared_ptr<ContainerFunction<QString, QString>>(
      new ContainerFunction<QString, QString>("icons8-ksi-50.png", "Threshold", "Threshold algorithm", "", "MANUAL",
                                              {{"MANUAL", "Manual"},
                                               {"LI", "Li"},
                                               {"MIN_ERROR", "Min. error"},
                                               {"TRIANGLE", "Triangle"},
                                               {"MOMENTS", "Moments"}}));

  mThresholdValueMin = std::shared_ptr<ContainerFunction<int, int>>(new ContainerFunction<int, int>(
      "icons8-grayscale-50.png", "[0 - 65535]", "Min. threshold", "px", 1000, 0, 65535));

  mMinProbability = std::shared_ptr<ContainerFunction<float, float>>(
      new ContainerFunction<float, float>("icons8-percentage-50.png", "[0 - 1]", "Min. probability", "%", 0.5, 0, 1));

  auto foundAIModels = joda::onnx::Onnx::findOnnxFiles();
  std::vector<ContainerFunction<QString, QString>::ComboEntry> aiModelsConverted;
  aiModelsConverted.reserve(foundAIModels.size());
  for(const auto &[path, model] : foundAIModels) {
    aiModelsConverted.push_back(
        ContainerFunction<QString, QString>::ComboEntry{.key = path.data(), .label = path.data()});
  }

  mAIModels = std::shared_ptr<ContainerFunction<QString, QString>>(new ContainerFunction<QString, QString>(
      "icons8-mind-map-50.png", "AI model", "AI model", "", "", aiModelsConverted));

  mMinCircularity = std::shared_ptr<ContainerFunction<float, float>>(new ContainerFunction<float, float>(
      "icons8-ellipse-50.png", "[0 - 1]", "Min. circularity", "%", std::nullopt, 0, 1));

  mMinParticleSize = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]",
                                      "Min. particle size", "px", 1, 0, INT32_MAX));
  mMaxParticleSize = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]",
                                      "Max. particle size", "px", std::nullopt, 0, INT32_MAX));

  mSnapAreaSize = std::shared_ptr<ContainerFunction<int, int>>(new ContainerFunction<int, int>(
      "icons8-initial-state-50.png", "[0 - 65535]", "Snap area size", "px", std::nullopt, 0, 65535));

  mZProjection = std::shared_ptr<ContainerFunction<QString, QString>>(new ContainerFunction<QString, QString>(
      "icons8-layers-50.png", "Z-Projection", "Z-Projection", "", "OFF",
      {{"OFF", "Off"}, {"MAX_INTENSITY", "Max. intensity"}, {"PROJECTION_3D", "3D projection"}}));

  mMarginCrop = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-crop-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Crop margin",
                                      "px", std::nullopt, 0, INT32_MAX));
  mSubtractChannel = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-layers-50.png", "Index", "Subtract other channel", "", -1,
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

  mMedianBackgroundSubtraction = std::shared_ptr<ContainerFunction<bool, bool>>(
      new ContainerFunction<bool, bool>("icons8-baseline-50.png", "On/Off", "Median background subtraction", false));
  mRollingBall = std::shared_ptr<ContainerFunction<int, QString>>(new ContainerFunction<int, QString>(
      "icons8-bubble-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Rolling ball", "px", std::nullopt, 0,
      INT32_MAX, {{"BALL", "Ball"}, {"PARABOLOID", "Paraboloid"}}, "BALL"));

  mGaussianBlur = std::shared_ptr<ContainerFunction<int, int>>(new ContainerFunction<int, int>(
      "icons8-blur-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Gaussian blur", "px", -1,
      {{-1, "Off"}, {3, "3x3"}, {5, "5x5"}, {7, "7x7"}}, {{1, "1x"}, {2, "2x"}, {3, "3x"}}, 1));

  mSmoothing = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-cleanup-noise-50.png", "Kernel size", "Smoothing", "", -1,
                                      {{-1, "Off"},
                                       {1, "x1"},
                                       {2, "x2"},
                                       {3, "x3"},
                                       {4, "x4"},
                                       {5, "x5"},
                                       {6, "x6"},
                                       {7, "x7"},
                                       {8, "x8"},
                                       {9, "x9"}}));
  mEdgeDetection     = std::shared_ptr<ContainerFunction<QString, QString>>(new ContainerFunction<QString, QString>(
      "icons8-triangle-50.png", "Threshold", "Edge detection", "", "NONE",
      {{"NONE", "Off"}, {"SOBEL", "Sobel"}, {"CANNY", "Canny"}}, {{"XY", "xy"}, {"X", "x"}, {"Y", "y"}}, "XY"));
  mTetraspeckRemoval = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-final-state-50.png", "Index", "Tetraspeck removal", "", -1,
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

  //
  // Cross channel
  //
  mColocGroup = std::shared_ptr<ContainerFunction<QString, int>>(new ContainerFunction<QString, int>(
      "icons8-query-outer-join-left-50.png", "Group", "Coloc group and min. overlap", "", "NONE",
      {{"NONE", "Off"}, {"A", "A"}, {"B", "B"}, {"C", "C"}},
      {{0, "0%"},
       {10, "10%"},
       {20, "20%"},
       {30, "30%"},
       {40, "40%"},
       {50, "50%"},
       {60, "60%"},
       {70, "70%"},
       {80, "80%"},
       {90, "90%"},
       {100, "100%"}},
      80));

  mCrossChannelIntensity = std::shared_ptr<ContainerFunction<QString, int>>(
      new ContainerFunction<QString, int>("icons8-light-50.png", "[0,1,2,3,..]", "Cross channel intensity", ""));

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
ContainerChannel::~ContainerChannel()
{
  delete mPanelOverview;
  delete mPanelEdit;
}

///
/// \brief      Load values
/// \author     Joachim Danmayr
///
void ContainerChannel::fromJson(std::optional<joda::settings::json::ChannelSettings> chSettings,
                                std::optional<joda::settings::json::PipelineStepVoronoi>)
{
  if(chSettings.has_value()) {
    // Meta
    mChannelType->setValue(QString(chSettings->getChannelInfo().getTypeString().data()));
    mChannelName->setValue(QString(chSettings->getChannelInfo().getName().data()));
    mColorAndChannelIndex->setValue(QString(chSettings->getChannelInfo().getColor().data()));
    mColorAndChannelIndex->setValueSecond(chSettings->getChannelInfo().getChannelIndex());

    mThresholdAlgorithm->clearValue();
    mThresholdValueMin->clearValue();
    mMinCircularity->clearValue();
    mMinParticleSize->clearValue();
    mMaxParticleSize->clearValue();
    mSnapAreaSize->clearValue();

    mZProjection->clearValue();
    mMarginCrop->clearValue();
    mSubtractChannel->clearValue();
    mMedianBackgroundSubtraction->clearValue();
    mRollingBall->clearValue();
    mGaussianBlur->clearValue();
    mSmoothing->clearValue();
    mEdgeDetection->clearValue();
    mTetraspeckRemoval->clearValue();

    mUsedDetectionMode->clearValue();
    mMinProbability->clearValue();
    mAIModels->clearValue();

    mColocGroup->clearValue();
    mCrossChannelIntensity->clearValue();

    // Preprocessing
    for(const auto &prepro : chSettings->getPreprocessingFunctions()) {
      if(prepro.getZStack()) {
        mZProjection->setValue(QString(prepro.getZStack()->value.data()));
      }
      if(prepro.getRollingBall()) {
        mRollingBall->setValue(prepro.getRollingBall()->value);
        mRollingBall->setValueSecond(prepro.getRollingBall()->mode.data());
      }
      if(prepro.getMarginCrop()) {
        mMarginCrop->setValue(prepro.getMarginCrop()->value);
      }
      if(prepro.getGaussianBlur()) {
        mGaussianBlur->setValue(prepro.getGaussianBlur()->kernel_size);
        mGaussianBlur->setValueSecond(prepro.getGaussianBlur()->repeat);
      }
      if(prepro.getSmoothing()) {
        mSmoothing->setValue(prepro.getSmoothing()->repeat);
      }
      if(prepro.getMedianBgSubtraction()) {
        mMedianBackgroundSubtraction->setValue(true);
      }
      if(prepro.getSubtractChannel()) {
        mSubtractChannel->setValue(prepro.getSubtractChannel()->channel_index);
      }
      if(prepro.getEdgeDetection()) {
        mEdgeDetection->setValue(prepro.getEdgeDetection()->value.data());
        mEdgeDetection->setValueSecond(prepro.getEdgeDetection()->direction.data());
      }
    }

    // Detection
    mUsedDetectionMode->setValue(chSettings->getDetectionSettings().getDetectionModeString().data());
    mThresholdAlgorithm->setValue(
        chSettings->getDetectionSettings().getThersholdSettings().getThresholdString().data());
    mThresholdValueMin->setValue(chSettings->getDetectionSettings().getThersholdSettings().getThresholdMin());
    mAIModels->setValue(chSettings->getDetectionSettings().getAiSettings().getModelName().data());
    mMinProbability->setValue(chSettings->getDetectionSettings().getAiSettings().getProbability());

    // Filtering
    mMinParticleSize->setValue(chSettings->getFilter().getMinParticleSize());
    if(chSettings->getFilter().getMaxParticleSize() >= INT32_MAX) {
      mMaxParticleSize->clearValue();
    } else {
      mMaxParticleSize->setValue(chSettings->getFilter().getMaxParticleSize());
    }
    mMinCircularity->setValue(chSettings->getFilter().getMinCircularity());
    mSnapAreaSize->setValue(chSettings->getFilter().getSnapAreaSize());
    mTetraspeckRemoval->setValue(chSettings->getFilter().getReferenceSpotChannelIndex());

    // Coloc
    auto &coloc = chSettings->getCrossChannelSettings().getColocGroups();
    if(coloc.size() > 0) {
      std::string value = *coloc.begin();
      mColocGroup->setValue(value.data());
      mColocGroup->setValueSecond(static_cast<int>(chSettings->getCrossChannelSettings().getMinColocArea() * 100.0F));
    }

    // Cross channel intensity
    auto &crossChannelIntensityChannels = chSettings->getCrossChannelSettings().getCrossChannelIntensityChannels();
    QString crossChannelIndexes;
    for(const auto chIdx : crossChannelIntensityChannels) {
      crossChannelIndexes += QString::number(chIdx) + ",";
    }
    if(crossChannelIndexes.size() > 0) {
      crossChannelIndexes.remove(crossChannelIndexes.lastIndexOf(","), 1);
    }
    mCrossChannelIntensity->setValue(crossChannelIndexes);
  }
}

///
/// \brief      Get values
/// \author     Joachim Danmayr
///
ContainerChannel::ConvertedChannels ContainerChannel::toJson() const
{
  nlohmann::json chSettings;

  chSettings["info"]["index"]  = mColorAndChannelIndex->getValueSecond();
  chSettings["info"]["series"] = mWindowMain->getSelectedSeries();
  chSettings["info"]["type"]   = mChannelType->getValue().toStdString();
  chSettings["info"]["label"]  = "";
  chSettings["info"]["name"]   = mChannelName->getValue().toStdString();
  chSettings["info"]["color"]  = mColorAndChannelIndex->getValue().toStdString();

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
    jsonArray.push_back({{"rolling_ball",
                          {{"value", static_cast<int>(mRollingBall->getValue())},
                           {"mode", mRollingBall->getValueSecond().toStdString()}}}});
  }

  if(mGaussianBlur->hasValue()) {
    jsonArray.push_back(
        {{"gaussian_blur", {{"kernel_size", mGaussianBlur->getValue()}, {"repeat", mGaussianBlur->getValue()}}}});
  }
  if(mSmoothing->hasValue()) {
    jsonArray.push_back({{"smoothing", {{"repeat", mSmoothing->getValue()}}}});
  }
  if(mMedianBackgroundSubtraction->hasValue() && mMedianBackgroundSubtraction->getValue()) {
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

  chSettings["detection"]["ai_settings"]["model_name"]      = mAIModels->getValue().toStdString();
  chSettings["detection"]["ai_settings"]["probability_min"] = mMinProbability->getValue();

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

  // Cross channel settings
  {
    std::set<std::string> colocGroup;
    if(mColocGroup->hasValue()) {
      colocGroup.emplace(mColocGroup->getValue().toStdString());
    }
    chSettings["cross_channel"]["coloc_groups"]   = colocGroup;
    chSettings["cross_channel"]["min_coloc_area"] = static_cast<float>(mColocGroup->getValueSecond()) / 100.0F;
  }
  {
    std::set<int32_t> crossChannelIntensity;
    auto values = mCrossChannelIntensity->getValue().split(",");
    for(const auto &val : values) {
      bool ok = false;
      auto i  = val.toInt(&ok);
      if(ok) {
        crossChannelIntensity.emplace(i);
      }
    }
    chSettings["cross_channel"]["cross_channel_intensity_channels"] = crossChannelIntensity;
  }

  return {.channelSettings = chSettings, .pipelineStepVoronoi = std::nullopt};
}

}    // namespace joda::ui::qt
