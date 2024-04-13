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

#include "container_voronoi.hpp"
#include <qwidget.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "../../window_main.hpp"
#include "../container_function.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "panel_voronoi_edit.hpp"
#include "panel_voronoi_overview.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerVoronoi::ContainerVoronoi(WindowMain *windowMain, joda::settings::VChannelVoronoi &settings) :
    mWindowMain(windowMain), mSettings(settings)
{
  mChannelName = std::shared_ptr<ContainerFunction<QString, QString>>(
      new ContainerFunction<QString, QString>("icons8-text-50.png", "Name", "Channel Name", "Name"));

  mChannelType = std::shared_ptr<ContainerFunction<joda::settings::ChannelSettingsMeta::Type, QString>>(
      new ContainerFunction<joda::settings::ChannelSettingsMeta::Type, QString>(
          "icons8-unknown-status-50.png", "Type", "Channel type", "", joda::settings::ChannelSettingsMeta::Type::SPOT,
          {{joda::settings::ChannelSettingsMeta::Type::SPOT, "Spot"},
           {joda::settings::ChannelSettingsMeta::Type::SPOT_REFERENCE, "Reference Spot"},
           {joda::settings::ChannelSettingsMeta::Type::NUCLEUS, "Nucleus"},
           {joda::settings::ChannelSettingsMeta::Type::CELL, "Cell"},
           {joda::settings::ChannelSettingsMeta::Type::BACKGROUND, "Background"}}));

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

  //
  // Cell approximation
  //
  mVoronoiPoints = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("dom-voronoi-50.png", "Indexes", "Voronoi points channel", "", -1,
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

  mMaxVoronoiAreaSize = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]",
                                      "Max. voronoi area radius", "px", 100, 0, INT32_MAX));

  mOverlayMaskChannelIndex = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-query-outer-join-50.png", "Indexes", "Overlay mask channel", "", -1,
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
  // Cross channel Intensity
  //
  mCrossChannelIntensity = std::shared_ptr<ContainerFunction<QString, int>>(
      new ContainerFunction<QString, int>("icons8-light-50.png", "[A,B,C,0,1,2,3,..]", "Cross channel intensity", ""));

  mCrossChannelCount = std::shared_ptr<ContainerFunction<QString, int>>(
      new ContainerFunction<QString, int>("icons8-3-50.png", "[A,B,C,0,1,2,3,..]", "Cross channel count", ""));

  //
  // Create panels -> Must be after creating the functions
  //
  mPanelOverview = new PanelVoronoiOverview(windowMain, this);
  mPanelEdit     = new PanelVoronoiEdit(windowMain, this);
}

///
/// \brief      Load values
/// \author     Joachim Danmayr
///
ContainerVoronoi::~ContainerVoronoi()
{
  delete mPanelOverview;
  delete mPanelEdit;
}

///
/// \brief      Load values
/// \author     Joachim Danmayr
///
void ContainerVoronoi::fromSettings()
{
  mMaxVoronoiAreaSize->clearValue();
  mCrossChannelIntensity->clearValue();

  // Meta
  mChannelType->setValue(mSettings.meta.type);
  mChannelName->setValue(mSettings.meta.name.data());
  mColorAndChannelIndex->setValue(mSettings.meta.color.data());
  mColorAndChannelIndex->setValueSecond(mSettings.meta.channelIdx);

  mVoronoiPoints->setValue(mSettings.voronoi.gridPointsChannelIdx);

  // Filtering
  mMaxVoronoiAreaSize->setValue(mSettings.voronoi.maxVoronoiAreaRadius);
  mOverlayMaskChannelIndex->setValue(mSettings.voronoi.overlayMaskChannelIdx);

  //
  // Cross channel
  //
  // Coloc

  // Cross channel intensity
  {
    auto &crossChannelIntensity = mSettings.crossChannel.crossChannelIntensityChannels;
    QString crossChannelIndexes;
    for(const auto chIdx : crossChannelIntensity) {
      crossChannelIndexes += QString::number(chIdx) + ",";
    }
    if(crossChannelIndexes.size() > 0) {
      crossChannelIndexes.remove(crossChannelIndexes.lastIndexOf(","), 1);
    }
    mCrossChannelIntensity->setValue(crossChannelIndexes);
  }

  // Cross channel count
  {
    auto &crosschannelCount = mSettings.crossChannel.crossChannelCoutChannels;
    QString crossChannelIndexes;
    for(const auto chIdx : crosschannelCount) {
      crossChannelIndexes += QString::number(chIdx) + ",";
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
void ContainerVoronoi::toSettings()
{
  mSettings.meta.channelIdx = mColorAndChannelIndex->getValueSecond();
  mSettings.meta.color      = mColorAndChannelIndex->getValue().toStdString();
  mSettings.meta.series     = mWindowMain->getSelectedSeries();
  mSettings.meta.type       = mChannelType->getValue();
  mSettings.meta.name       = mChannelName->getValue().toStdString();

  mSettings.voronoi.gridPointsChannelIdx  = mVoronoiPoints->getValue();
  mSettings.voronoi.overlayMaskChannelIdx = mOverlayMaskChannelIndex->getValue();
  mSettings.voronoi.maxVoronoiAreaRadius  = mMaxVoronoiAreaSize->getValue();

  // Cross channel settings
  {
    std::set<int32_t> crossChannelIntensity;
    auto values = mCrossChannelIntensity->getValue().split(",");
    for(const auto &val : values) {
      if(!val.isEmpty()) {
        crossChannelIntensity.emplace(val.toInt());
      }
    }
    mSettings.crossChannel.crossChannelIntensityChannels = crossChannelIntensity;
  }

  {
    std::set<int32_t> crossChannelCount;
    auto values = mCrossChannelCount->getValue().split(",");
    for(const auto &val : values) {
      if(!val.isEmpty()) {
        crossChannelCount.emplace(val.toInt());
      }
    }
    mSettings.crossChannel.crossChannelCoutChannels = crossChannelCount;
  }
}

}    // namespace joda::ui::qt
