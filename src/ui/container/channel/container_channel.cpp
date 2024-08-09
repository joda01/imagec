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
#include "../container_function.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/settings/preprocessing/functions/blur.hpp"
#include "backend/settings/preprocessing/functions/channel_subtaction.hpp"
#include "backend/settings/preprocessing/functions/edge_detection.hpp"
#include "backend/settings/preprocessing/functions/gaussian_blur.hpp"
#include "backend/settings/preprocessing/functions/margin_crop.hpp"
#include "backend/settings/preprocessing/functions/median_subtract.hpp"
#include "backend/settings/preprocessing/functions/rolling_ball.hpp"
#include "backend/settings/preprocessing/functions/zstack.hpp"
#include "ui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>
#include "panel_channel_overview.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerChannel::ContainerChannel(WindowMain *windowMain, joda::settings::ChannelSettings &settings) :
    mWindowMain(windowMain), mSettings(settings)
{
  mChannelName = std::shared_ptr<ContainerFunction<QString, QString>>(new ContainerFunction<QString, QString>(
      "icons8-text-50.png", "Name", "Name", "Name", windowMain, "channel_name.json"));
  mChannelName->setMaxLength(15);

  mChannelType = std::shared_ptr<ContainerFunction<joda::settings::ChannelSettingsMeta::Type, QString>>(
      new ContainerFunction<joda::settings::ChannelSettingsMeta::Type, QString>(
          "icons8-unknown-status-50.png", "Type", "Type", "", joda::settings::ChannelSettingsMeta::Type::SPOT,
          {{joda::settings::ChannelSettingsMeta::Type::SPOT, "Spot"},
           {joda::settings::ChannelSettingsMeta::Type::SPOT_REFERENCE, "Reference Spot"},
           {joda::settings::ChannelSettingsMeta::Type::NUCLEUS, "Nucleus"},
           {joda::settings::ChannelSettingsMeta::Type::CELL, "Cell"},
           {joda::settings::ChannelSettingsMeta::Type::BACKGROUND, "Background"}},
          windowMain, "channel_type.json"));

  mColorAndChannelIndex = std::shared_ptr<ContainerFunction<QString, joda::settings::ChannelIndex>>(
      new ContainerFunction<QString, joda::settings::ChannelIndex>(
          "icons8-unknown-status-50.png", "Type", "Index", "", "#B91717",
          {{"#B91717", "", "icons8-bubble-50red-#B91717.png"},
           {"#06880C", "", "icons8-bubble-50 -green-#06880C.png"},
           {"#1771B9", "", "icons8-bubble-blue-#1771B9-50.png"},
           {"#FBEA25", "", "icons8-bubble-50-yellow-#FBEA25.png"},
           {"#6F03A6", "", "icons8-bubble-50-violet-#6F03A6.png"},
           {"#818181", "", "icons8-bubble-50-gray-#818181.png"},
           /*{"#000000", "", "icons8-bubble-50-black-#000000.png"}*/},
          {{joda::settings::ChannelIndex::CH0, "Channel 0"},
           {joda::settings::ChannelIndex::CH1, "Channel 1"},
           {joda::settings::ChannelIndex::CH2, "Channel 2"},
           {joda::settings::ChannelIndex::CH3, "Channel 3"},
           {joda::settings::ChannelIndex::CH4, "Channel 4"},
           {joda::settings::ChannelIndex::CH5, "Channel 5"},
           {joda::settings::ChannelIndex::CH6, "Channel 6"},
           {joda::settings::ChannelIndex::CH7, "Channel 7"},
           {joda::settings::ChannelIndex::CH8, "Channel 8"},
           {joda::settings::ChannelIndex::CH9, "Channel 9"}},
          joda::settings::ChannelIndex::CH0, windowMain, "channel_index.json"));

  mUsedDetectionMode = std::shared_ptr<ContainerFunction<joda::settings::DetectionSettings::DetectionMode, QString>>(
      new ContainerFunction<joda::settings::DetectionSettings::DetectionMode, QString>(
          "icons8-background-remover-50.png", "Threshold", "Detection mode", "",
          joda::settings::DetectionSettings::DetectionMode::THRESHOLD,
          {{joda::settings::DetectionSettings::DetectionMode::THRESHOLD, "Threshold"},
           {joda::settings::DetectionSettings::DetectionMode::AI, "Artificial intelligence"}},
          windowMain, "detection_mode.json"));

  mThresholdAlgorithm = std::shared_ptr<ContainerFunction<joda::settings::ThresholdSettings::Mode, QString>>(
      new ContainerFunction<joda::settings::ThresholdSettings::Mode, QString>(
          "icons8-lambda-50.png", "Threshold", "Threshold algorithm", "",
          joda::settings::ThresholdSettings::Mode::MANUAL,
          {{joda::settings::ThresholdSettings::Mode::MANUAL, "Manual"},
           {joda::settings::ThresholdSettings::Mode::LI, "Li"},
           {joda::settings::ThresholdSettings::Mode::MIN_ERROR, "Min. error"},
           {joda::settings::ThresholdSettings::Mode::TRIANGLE, "Triangle"},
           {joda::settings::ThresholdSettings::Mode::MOMENTS, "Moments"},
           {joda::settings::ThresholdSettings::Mode::OTSU, "Otsu"}},
          windowMain, "threshold_algorithm.json"));

  mThresholdValueMin = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-grayscale-50.png", "[0 - 65535]", "Min. threshold", "px", 1000, 0, 65535,
                                      windowMain, "threshold_min.json"));

  mMinProbability = std::shared_ptr<ContainerFunction<float, float>>(
      new ContainerFunction<float, float>("icons8-percentage-50.png", "[0 - 1]", "Min. probability", "%", 0.5, 0, 1,
                                          windowMain, "ai_min_probability.json"));

  mWateredSegmentation = std::shared_ptr<ContainerFunction<bool, bool>>(
      new ContainerFunction<bool, bool>("icons8-split-50", "Watershed segmentation", "Watershed segmentation", false,
                                        windowMain, "watershed_segmentation.json"));

  auto foundAIModels = joda::onnx::OnnxParser::findOnnxFiles();
  std::vector<ContainerFunction<QString, QString>::ComboEntry> aiModelsConverted;
  aiModelsConverted.reserve(foundAIModels.size());
  for(const auto &[path, model] : foundAIModels) {
    aiModelsConverted.push_back(
        ContainerFunction<QString, QString>::ComboEntry{.key = path.data(), .label = path.data()});
  }

  mAIModels = std::shared_ptr<ContainerFunction<QString, QString>>(new ContainerFunction<QString, QString>(
      "icons8-mind-map-50.png", "AI model", "AI model", "", "", aiModelsConverted, windowMain, "ai_model.json"));

  mMinCircularity = std::shared_ptr<ContainerFunction<float, float>>(
      new ContainerFunction<float, float>("icons8-ellipse-50.png", "[0 - 1]", "Min. circularity", "%", std::nullopt, 0,
                                          1, windowMain, "min_circularity.json"));

  mMinParticleSize = std::shared_ptr<ContainerFunction<int, int>>(new ContainerFunction<int, int>(
      "icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Min. particle size", "px", 1, 0, INT32_MAX,
      windowMain, "min_particle_size.json"));
  mMaxParticleSize = std::shared_ptr<ContainerFunction<int, int>>(new ContainerFunction<int, int>(
      "icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Max. particle size", "px", std::nullopt, 0,
      INT32_MAX, windowMain, "max_particle_size.json"));

  mSnapAreaSize = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-initial-state-50.png", "[0 - 65535]", "Snap area size", "px",
                                      std::nullopt, 0, 65535, windowMain, "snap_area_size.json"));

  mZProjection = std::shared_ptr<ContainerFunction<joda::settings::ZStackProcessing::ZStackMethod, QString>>(
      new ContainerFunction<joda::settings::ZStackProcessing::ZStackMethod, QString>(
          "icons8-layers-50.png", "Z-Projection", "Z-Projection", "",
          joda::settings::ZStackProcessing::ZStackMethod::NONE,
          {{joda::settings::ZStackProcessing::ZStackMethod::NONE, "Off"},
           {joda::settings::ZStackProcessing::ZStackMethod::MAX_INTENSITY, "Max. intensity"},
           {joda::settings::ZStackProcessing::ZStackMethod::PROJECT_3D, "3D projection"}},
          windowMain, "z_projection.json"));

  mMarginCrop = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-crop-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Crop margin",
                                      "px", std::nullopt, 0, INT32_MAX, windowMain, "margin_crop.json"));
  mSubtractChannel = std::shared_ptr<ContainerFunction<joda::settings::ChannelIndex, int>>(
      new ContainerFunction<joda::settings::ChannelIndex, int>("icons8-layers-50.png", "Index", "Subtract channel", "",
                                                               joda::settings::ChannelIndex::NONE,
                                                               {{joda::settings::ChannelIndex::NONE, "Off"},
                                                                {joda::settings::ChannelIndex::CH0, "Channel 0"},
                                                                {joda::settings::ChannelIndex::CH1, "Channel 1"},
                                                                {joda::settings::ChannelIndex::CH2, "Channel 2"},
                                                                {joda::settings::ChannelIndex::CH3, "Channel 3"},
                                                                {joda::settings::ChannelIndex::CH4, "Channel 4"},
                                                                {joda::settings::ChannelIndex::CH5, "Channel 5"},
                                                                {joda::settings::ChannelIndex::CH6, "Channel 6"},
                                                                {joda::settings::ChannelIndex::CH7, "Channel 7"},
                                                                {joda::settings::ChannelIndex::CH8, "Channel 8"},
                                                                {joda::settings::ChannelIndex::CH9, "Channel 9"}},
                                                               windowMain, "subtract_channel.json"));

  mMedianBackgroundSubtraction = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-baseline-50.png", "Kernel size", "Median bg. subtraction", "", -1,
                                      {{-1, "Off"},
                                       {3, "3x3"},
                                       {4, "4x4"},
                                       {5, "5x5"},
                                       {7, "7x7"},
                                       {9, "9x9"},
                                       {11, "11x11"},
                                       {13, "13x13"},
                                       {15, "15x15"},
                                       {17, "17x17"},
                                       {19, "19x19"},
                                       {21, "21x21"},
                                       {23, "23x23"}},
                                      windowMain, "median_background_subtraction.json"));

  mRollingBall = std::shared_ptr<ContainerFunction<int, joda::settings::RollingBall::BallType>>(
      new ContainerFunction<int, joda::settings::RollingBall::BallType>(
          "icons8-bubble-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Rolling ball", "px", std::nullopt, 0,
          INT32_MAX,
          {{joda::settings::RollingBall::BallType::BALL, "Ball"},
           {joda::settings::RollingBall::BallType::PARABOLOID, "Paraboloid"}},
          joda::settings::RollingBall::BallType::BALL, windowMain, "rolling_ball.json"));

  mGaussianBlur = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-blur-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Gaussian blur",
                                      "px", -1, {{-1, "Off"}, {3, "3x3"}, {5, "5x5"}, {7, "7x7"}, {9, "9x9"}},
                                      {{1, "1x"}, {2, "2x"}, {3, "3x"}}, 1, windowMain, "gaussian_blur.json"));

  mSmoothing = std::shared_ptr<ContainerFunction<int, int>>(new ContainerFunction<int, int>(
      "icons8-cleanup-noise-50.png", "Kernel size", "Smoothing", "", -1,
      {{-1, "Off"}, {1, "x1"}, {2, "x2"}, {3, "x3"}, {4, "x4"}, {5, "x5"}, {6, "x6"}, {7, "x7"}, {8, "x8"}, {9, "x9"}},
      windowMain, "smoothing.json"));
  mEdgeDetection =
      std::shared_ptr<ContainerFunction<joda::settings::EdgeDetection::Mode, joda::settings::EdgeDetection::Direction>>(
          new ContainerFunction<joda::settings::EdgeDetection::Mode, joda::settings::EdgeDetection::Direction>(
              "icons8-triangle-50.png", "Threshold", "Edge detection", "", joda::settings::EdgeDetection::Mode::NONE,
              {{joda::settings::EdgeDetection::Mode::NONE, "Off"},
               {joda::settings::EdgeDetection::Mode::SOBEL, "Sobel"},
               {joda::settings::EdgeDetection::Mode::CANNY, "Canny"}},
              {{joda::settings::EdgeDetection::Direction::XY, "xy"},
               {joda::settings::EdgeDetection::Direction::X, "x"},
               {joda::settings::EdgeDetection::Direction::Y, "y"}},
              joda::settings::EdgeDetection::Direction::XY, windowMain, "edge_detection.json"));
  mTetraspeckRemoval = std::shared_ptr<ContainerFunction<joda::settings::ChannelIndex, int>>(
      new ContainerFunction<joda::settings::ChannelIndex, int>(
          "icons8-minus-sign-50.png", "Index", "Tetraspeck removal", "", joda::settings::ChannelIndex::NONE,
          {{joda::settings::ChannelIndex::NONE, "Off"},
           {joda::settings::ChannelIndex::CH0, "Channel 0"},
           {joda::settings::ChannelIndex::CH1, "Channel 1"},
           {joda::settings::ChannelIndex::CH2, "Channel 2"},
           {joda::settings::ChannelIndex::CH3, "Channel 3"},
           {joda::settings::ChannelIndex::CH4, "Channel 4"},
           {joda::settings::ChannelIndex::CH5, "Channel 5"},
           {joda::settings::ChannelIndex::CH6, "Channel 6"},
           {joda::settings::ChannelIndex::CH7, "Channel 7"},
           {joda::settings::ChannelIndex::CH8, "Channel 8"},
           {joda::settings::ChannelIndex::CH9, "Channel 9"}},
          windowMain, "tetraspeck_removal.json"));

  mCrossChannelIntensity = std::shared_ptr<ContainerFunction<QString, int>>(new ContainerFunction<QString, int>(
      "icons8-light-50.png", "[A,B,C,0,1,2,3,..]", "Cross intensity", "", windowMain, "cross_channel_intensity.json"));

  mCrossChannelCount = std::shared_ptr<ContainerFunction<QString, int>>(new ContainerFunction<QString, int>(
      "icons8-3-50.png", "[A,B,C,0,1,2,3,..]", "Cross count", "", windowMain, "cross_channel_count.json"));

  //
  // Cross-Channel
  //
  mImageFilterMode = std::shared_ptr<ContainerFunction<joda::settings::ChannelImageFilter::FilterMode, int>>(
      new ContainerFunction<joda::settings::ChannelImageFilter::FilterMode, int>(
          "icons8-filter-50.png", "Index", "Image filter mode", "", joda::settings::ChannelImageFilter::FilterMode::OFF,
          {{joda::settings::ChannelImageFilter::FilterMode::OFF, "Off"},
           {joda::settings::ChannelImageFilter::FilterMode::INVALIDATE_CHANNEL, "Invalidate channel"},
           {joda::settings::ChannelImageFilter::FilterMode::INVALIDATE_WHOLE_IMAGE, "Invalidate whole image"}},
          windowMain, "image_filter_mode.json"));

  mMaxObjects = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-infinity-50.png", "[0 - 2147483647]", "Max. objects", "", std::nullopt, 0,
                                      2147483647, windowMain, "image_filter_max_objects.json"));

  mHistogramThresholdFactor = std::shared_ptr<ContainerFunction<float, float>>(new ContainerFunction<float, float>(
      "icons8-histogram-50-01.png", "[0 - 1]", "Hist. threshold factor", "", std::nullopt, 0, 1, windowMain,
      "image_filter_histogram_threshold_filter.json"));

  //
  // Create panels -> Must be after creating the functions
  //
  mPanelEdit     = new PanelChannelEdit(windowMain, this);
  mPanelOverview = new PanelChannelOverview(windowMain, this);
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
void ContainerChannel::fromSettings()
{
  // Meta
  mChannelType->setValue(mSettings.meta.type);
  mChannelName->setValue(mSettings.meta.name.data());
  mColorAndChannelIndex->setValue(mSettings.meta.color.data());
  mColorAndChannelIndex->setValueSecond(mSettings.meta.channelIdx);

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
  mWateredSegmentation->clearValue();

  mCrossChannelIntensity->clearValue();

  // Image filter
  mImageFilterMode->clearValue();
  mMaxObjects->clearValue();
  mHistogramThresholdFactor->clearValue();

  mZProjection->setValue(mSettings.preprocessing.$zStack.method);

  if(mSettings.preprocessing.$cropMargin.has_value()) {
    mMarginCrop->setValue(mSettings.preprocessing.$cropMargin->marginSize);
  }

  // Preprocessing
  for(const auto &prepro : mSettings.preprocessing.pipeline) {
    if(prepro.$medianSubtract.has_value()) {
      mMedianBackgroundSubtraction->setValue(prepro.$medianSubtract->kernelSize);
    }

    if(prepro.$rollingBall.has_value()) {
      mRollingBall->setValue(prepro.$rollingBall->ballSize);
      mRollingBall->setValueSecond(prepro.$rollingBall->ballType);
    }
    if(prepro.$gaussianBlur.has_value()) {
      mGaussianBlur->setValue(prepro.$gaussianBlur->kernelSize);
      mGaussianBlur->setValueSecond(prepro.$gaussianBlur->repeat);
    }
    if(prepro.$blur) {
      mSmoothing->setValue(prepro.$blur->repeat);
    }
    if(prepro.$subtractChannel.has_value()) {
      mSubtractChannel->setValue(prepro.$subtractChannel->channelIdx);
    }
    if(prepro.$edgeDetection.has_value()) {
      mEdgeDetection->setValue(prepro.$edgeDetection->mode);
      mEdgeDetection->setValueSecond(prepro.$edgeDetection->direction);
    }
  }

  // Detection
  mUsedDetectionMode->setValue(mSettings.detection.detectionMode);
  mThresholdAlgorithm->setValue(mSettings.detection.threshold.mode);
  mThresholdValueMin->setValue(mSettings.detection.threshold.thresholdMin);
  mAIModels->setValue(mSettings.detection.ai.modelPath.data());
  mMinProbability->setValue(mSettings.detection.ai.minProbability);
  mWateredSegmentation->setValue(mSettings.detection.threshold.$watershedSegmentation.enabled);

  // Filtering
  mMinParticleSize->setValue(mSettings.objectFilter.minParticleSize);
  if(mSettings.objectFilter.maxParticleSize >= INT32_MAX) {
    mMaxParticleSize->clearValue();
  } else {
    mMaxParticleSize->setValue(mSettings.objectFilter.maxParticleSize);
  }
  mMinCircularity->setValue(mSettings.objectFilter.minCircularity);
  mSnapAreaSize->setValue(mSettings.objectFilter.snapAreaSize);
  mTetraspeckRemoval->setValue(mSettings.objectFilter.referenceSpotChannelIndex);

  // Image filter
  mImageFilterMode->setValue(mSettings.imageFilter.filterMode);
  if(mSettings.imageFilter.maxObjects > 0) {
    mMaxObjects->setValue(mSettings.imageFilter.maxObjects);
  }
  if(mSettings.imageFilter.histMinThresholdFilterFactor > 0) {
    mHistogramThresholdFactor->setValue(mSettings.imageFilter.histMinThresholdFilterFactor);
  }

  // Cross channel intensity
  {
    auto &crossChannelIntensity = mSettings.crossChannel.crossChannelIntensityChannels;
    QString crossChannelIndexes;
    for(const auto chIdx : crossChannelIntensity) {
      if(static_cast<int32_t>(chIdx) < 65) {
        crossChannelIndexes += QString::number(static_cast<int32_t>(chIdx)) + ",";
      } else {
        char ch = static_cast<char>(chIdx);
        crossChannelIndexes += QString(std::to_string(ch).data()) + ",";
      }
    }
    if(crossChannelIndexes.size() > 0) {
      crossChannelIndexes.remove(crossChannelIndexes.lastIndexOf(","), 1);
    }
    mCrossChannelIntensity->setValue(crossChannelIndexes);
  }

  // Cross channel count
  {
    auto &crosschannelCount = mSettings.crossChannel.crossChannelCountChannels;
    QString crossChannelIndexes;
    for(const auto chIdx : crosschannelCount) {
      if(static_cast<int32_t>(chIdx) < 65) {
        crossChannelIndexes += QString::number(static_cast<int32_t>(chIdx)) + ",";
      } else {
        char ch = static_cast<char>(chIdx);
        crossChannelIndexes += QString(std::to_string(ch).data()) + ",";
      }
    }
    if(crossChannelIndexes.size() > 0) {
      crossChannelIndexes.remove(crossChannelIndexes.lastIndexOf(","), 1);
    }
    mCrossChannelCount->setValue(crossChannelIndexes);
  }
}

///
/// \brief      Get values
/// \author     Joachim Danmayr
///
void ContainerChannel::toSettings()
{
  mSettings.meta.channelIdx = mColorAndChannelIndex->getValueSecond();
  mSettings.meta.color      = mColorAndChannelIndex->getValue().toStdString();
  mSettings.meta.series     = mWindowMain->getSelectedSeries();
  mSettings.meta.type       = mChannelType->getValue();
  mSettings.meta.name       = mChannelName->getValue().toStdString();

  mSettings.preprocessing.$zStack     = settings::ZStackProcessing{.method = mZProjection->getValue()};
  mSettings.preprocessing.$cropMargin = settings::MarginCrop{.marginSize = mMarginCrop->getValue()};

  mSettings.preprocessing.pipeline.clear();
  //
  //
  if(mSubtractChannel->hasValue()) {
    settings::PreprocessingPipelineSteps step;
    step.$subtractChannel = joda::settings::ChannelSubtraction{.channelIdx = mSubtractChannel->getValue()};
    mSettings.preprocessing.pipeline.push_back(step);
  }

  //
  //
  if(mMedianBackgroundSubtraction->hasValue()) {
    settings::PreprocessingPipelineSteps step;
    step.$medianSubtract = joda::settings::MedianSubtraction{.kernelSize = mMedianBackgroundSubtraction->getValue()};
    mSettings.preprocessing.pipeline.push_back(step);
  }

  //
  //
  if(mEdgeDetection->getValue() != joda::settings::EdgeDetection::Mode::NONE) {
    settings::PreprocessingPipelineSteps step;
    step.$edgeDetection = joda::settings::EdgeDetection{.mode      = mEdgeDetection->getValue(),
                                                        .direction = mEdgeDetection->getValueSecond()};
    mSettings.preprocessing.pipeline.push_back(step);
  }

  //
  //
  if(mRollingBall->getValue() > 0) {
    settings::PreprocessingPipelineSteps step;
    step.$rollingBall =
        joda::settings::RollingBall{.ballSize = mRollingBall->getValue(), .ballType = mRollingBall->getValueSecond()};
    mSettings.preprocessing.pipeline.push_back(step);
  }

  //
  //
  if(mGaussianBlur->hasValue()) {
    settings::PreprocessingPipelineSteps step;
    step.$gaussianBlur = joda::settings::GaussianBlur{.kernelSize = mGaussianBlur->getValue(),
                                                      .repeat     = mGaussianBlur->getValueSecond()};
    mSettings.preprocessing.pipeline.push_back(step);
  }

  //
  //
  if(mSmoothing->hasValue()) {
    settings::PreprocessingPipelineSteps step;
    step.$blur = joda::settings::Blur{.kernelSize = 3, .repeat = mSmoothing->getValue()};
    mSettings.preprocessing.pipeline.push_back(step);
  }

  // Detections
  mSettings.detection.detectionMode = mUsedDetectionMode->getValue();

  mSettings.detection.threshold.mode                           = mThresholdAlgorithm->getValue();
  mSettings.detection.threshold.thresholdMin                   = mThresholdValueMin->getValue();
  mSettings.detection.threshold.thresholdMax                   = UINT16_MAX;
  mSettings.detection.threshold.$watershedSegmentation.enabled = mWateredSegmentation->getValue();

  mSettings.detection.ai.modelPath      = mAIModels->getValue().toStdString();
  mSettings.detection.ai.minProbability = mMinProbability->getValue();

  // Filtering
  if(mMinParticleSize->hasValue()) {
    mSettings.objectFilter.minParticleSize = mMinParticleSize->getValue();
  } else {
    mSettings.objectFilter.minParticleSize = 0;
  }

  if(mMaxParticleSize->hasValue()) {
    mSettings.objectFilter.maxParticleSize = mMaxParticleSize->getValue();
  } else {
    mSettings.objectFilter.maxParticleSize = INT32_MAX;
  }

  mSettings.objectFilter.minCircularity            = mMinCircularity->getValue();
  mSettings.objectFilter.snapAreaSize              = mSnapAreaSize->getValue();
  mSettings.objectFilter.referenceSpotChannelIndex = mTetraspeckRemoval->getValue();

  // Image filter
  mSettings.imageFilter.filterMode = mImageFilterMode->getValue();
  if(mMaxObjects->hasValue()) {
    mSettings.imageFilter.maxObjects = mMaxObjects->getValue();
  } else {
    mSettings.imageFilter.maxObjects = -1;
  }

  if(mHistogramThresholdFactor->hasValue()) {
    mSettings.imageFilter.histMinThresholdFilterFactor = mHistogramThresholdFactor->getValue();
  } else {
    mSettings.imageFilter.histMinThresholdFilterFactor = -1;
  }

  // Cross channel settings
  {
    std::set<joda::settings::ChannelIndex> crossChannelIntensity;
    auto values = mCrossChannelIntensity->getValue().split(",");
    for(const auto &val : values) {
      if(!val.isEmpty()) {
        if(val[0] < 'A') {
          crossChannelIntensity.emplace((joda::settings::ChannelIndex) val.toInt());
        } else {
          crossChannelIntensity.emplace((joda::settings::ChannelIndex) val[0].toLatin1());
        }
      }
    }
    mSettings.crossChannel.crossChannelIntensityChannels = crossChannelIntensity;
  }

  {
    std::set<joda::settings::ChannelIndex> crossChannelCount;
    auto values = mCrossChannelCount->getValue().split(",");
    for(const auto &val : values) {
      if(!val.isEmpty()) {
        if(val[0] < 'A') {
          crossChannelCount.emplace((joda::settings::ChannelIndex) val.toInt());
        } else {
          crossChannelCount.emplace((joda::settings::ChannelIndex) val[0].toLatin1());
        }
      }
    }
    mSettings.crossChannel.crossChannelCountChannels = crossChannelCount;
  }
}

}    // namespace joda::ui::qt
