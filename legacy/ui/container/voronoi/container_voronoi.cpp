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
#include "../container_function.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "ui/window_main/window_main.hpp"
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
      new ContainerFunction<QString, QString>("icons8-text-50.png", "Name", "Channel Name", "Name", windowMain));
  mChannelName->setMaxLength(15);

  mChannelType = std::shared_ptr<ContainerFunction<joda::settings::ChannelSettingsMeta::Type, QString>>(
      new ContainerFunction<joda::settings::ChannelSettingsMeta::Type, QString>(
          "icons8-unknown-status-50.png", "Type", "Channel type", "", joda::settings::ChannelSettingsMeta::Type::SPOT,
          {{joda::settings::ChannelSettingsMeta::Type::SPOT, "Spot"},
           {joda::settings::ChannelSettingsMeta::Type::SPOT_REFERENCE, "Reference Spot"},
           {joda::settings::ChannelSettingsMeta::Type::NUCLEUS, "Nucleus"},
           {joda::settings::ChannelSettingsMeta::Type::CELL, "Cell"},
           {joda::settings::ChannelSettingsMeta::Type::BACKGROUND, "Background"}},
          windowMain));

  mColorAndChannelIndex = std::shared_ptr<ContainerFunction<QString, joda::settings::ChannelIndex>>(
      new ContainerFunction<QString, joda::settings::ChannelIndex>(
          "icons8-unknown-status-50.png", "Type", "Channel index", "", "#B91717",
          {{"#B91717", "", "icons8-bubble-50red-#B91717.png"},
           {"#06880C", "", "icons8-bubble-50 -green-#06880C.png"},
           {"#1771B9", "", "icons8-bubble-blue-#1771B9-50.png"},
           {"#FBEA25", "", "icons8-bubble-50-yellow-#FBEA25.png"},
           {"#6F03A6", "", "icons8-bubble-50-violet-#6F03A6.png"},
           {"#818181", "", "icons8-bubble-50-gray-#818181.png"},
           /*{"#000000", "", "icons8-bubble-50-black-#000000.png"}*/},
          {{joda::settings::ChannelIndex::A, "Slot A"},
           {joda::settings::ChannelIndex::B, "Slot B"},
           {joda::settings::ChannelIndex::C, "Slot C"},
           {joda::settings::ChannelIndex::D, "Slot D"},
           {joda::settings::ChannelIndex::E, "Slot E"},
           {joda::settings::ChannelIndex::F, "Slot F"}},
          joda::settings::ChannelIndex::A, windowMain));

  //
  // Cell approximation
  //
  mVoronoiPoints = std::shared_ptr<ContainerFunction<joda::settings::ChannelIndex, int>>(
      new ContainerFunction<joda::settings::ChannelIndex, int>(
          "dom-voronoi-50.png", "Indexes", "Voronoi points channel", "", joda::settings::ChannelIndex::NONE,
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
          windowMain));

  mMaxVoronoiAreaSize = std::shared_ptr<ContainerFunction<int, int>>(
      new ContainerFunction<int, int>("icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]",
                                      "Max. voronoi area radius", "px", 100, 0, INT32_MAX, windowMain));

  mOverlayMaskChannelIndex = std::shared_ptr<ContainerFunction<joda::settings::ChannelIndex, int>>(
      new ContainerFunction<joda::settings::ChannelIndex, int>(
          "icons8-query-outer-join-50.png", "Indexes", "Overlay mask channel", "", joda::settings::ChannelIndex::NONE,
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
          windowMain));

  //
  // Filtering
  //
  mMinParticleSize = std::shared_ptr<ContainerFunction<int, int>>(new ContainerFunction<int, int>(
      "icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Min. particle size", "px", 1, 0, INT32_MAX,
      windowMain, "min_particle_size.json"));
  mMaxParticleSize = std::shared_ptr<ContainerFunction<int, int>>(new ContainerFunction<int, int>(
      "icons8-all-out-50.png", "[0 - " + QString::number(INT32_MAX) + "]", "Max. particle size", "px", std::nullopt, 0,
      INT32_MAX, windowMain, "max_particle_size.json"));

  mExcludeAreasWithoutCenterOfMass = std::shared_ptr<ContainerFunction<bool, bool>>(new ContainerFunction<bool, bool>(
      "icons8-body-cells-50.png", "Exclude areas without center of mass", "Exclude areas without center of mass", true,
      windowMain, "voronoi_exclude_areas_without_center_of_mass.json"));

  mExcludeAreasAtTheEdges = std::shared_ptr<ContainerFunction<bool, bool>>(new ContainerFunction<bool, bool>(
      "icons8-coupon-50.png", "Exclude areas at the edges", "Exclude areas at the edges", false, windowMain,
      "voronoi_exclude_areas_at_edges.json"));

  //
  // Cross channel Intensity
  //
  mCrossChannelIntensity = std::shared_ptr<ContainerFunction<QString, int>>(new ContainerFunction<QString, int>(
      "icons8-light-50.png", "[A,B,C,0,1,2,3,..]", "Cross channel intensity", "", windowMain));

  mCrossChannelCount = std::shared_ptr<ContainerFunction<QString, int>>(new ContainerFunction<QString, int>(
      "icons8-3-50.png", "[A,B,C,0,1,2,3,..]", "Cross channel count", "", windowMain));

  //
  // Create panels -> Must be after creating the functions
  //
  mPanelEdit     = new PanelVoronoiEdit(windowMain, this);
  mPanelOverview = new PanelVoronoiOverview(windowMain, this);
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
void ContainerVoronoi::fromSettings(const joda::settings::VChannelVoronoi &settings)
{
  mMaxVoronoiAreaSize->clearValue();
  mCrossChannelIntensity->clearValue();

  // Meta
  mChannelType->setValue(settings.meta.type);
  mChannelName->setValue(settings.meta.name.data());
  mColorAndChannelIndex->setValue(settings.meta.color.data());
  mColorAndChannelIndex->setValueSecond(settings.meta.channelIdx);

  mVoronoiPoints->setValue(settings.voronoi.gridPointsChannelIdx);

  // Filtering
  mMaxVoronoiAreaSize->setValue(settings.voronoi.maxVoronoiAreaRadius);
  mOverlayMaskChannelIndex->setValue(settings.voronoi.overlayMaskChannelIdx);

  mMinParticleSize->clearValue();
  mMaxParticleSize->clearValue();

  // Filtering
  mMinParticleSize->setValue(settings.objectFilter.minParticleSize);
  if(settings.objectFilter.maxParticleSize >= INT32_MAX) {
    mMaxParticleSize->clearValue();
  } else {
    mMaxParticleSize->setValue(settings.objectFilter.maxParticleSize);
  }
  mExcludeAreasWithoutCenterOfMass->setValue(settings.objectFilter.excludeAreasWithoutCenterOfMass);
  mExcludeAreasAtTheEdges->setValue(settings.objectFilter.excludeAreasAtEdges);

  //
  // Cross channel
  //

  // Cross channel intensity
  {
    auto &crossChannelIntensity = settings.crossChannel.crossChannelIntensityChannels;
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
    auto &crosschannelCount = settings.crossChannel.crossChannelCountChannels;
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
nlohmann::json ContainerVoronoi::toJson(const std::string &titlePrefix)
{
  toSettings();
  auto settings = mSettings;
  settings.meta.name += titlePrefix;
  return settings;
}

///
/// \brief      Get values
/// \author     Joachim Danmayr
///
void ContainerVoronoi::toSettings()
{
  auto [imgIdx, series] = mWindowMain->getImagePanel()->getSelectedImage();

  mSettings.meta.channelIdx = mColorAndChannelIndex->getValueSecond();
  mSettings.meta.color      = mColorAndChannelIndex->getValue().toStdString();
  mSettings.meta.series     = series;
  mSettings.meta.type       = mChannelType->getValue();
  mSettings.meta.name       = mChannelName->getValue().toStdString();

  mSettings.voronoi.gridPointsChannelIdx  = mVoronoiPoints->getValue();
  mSettings.voronoi.overlayMaskChannelIdx = mOverlayMaskChannelIndex->getValue();
  mSettings.voronoi.maxVoronoiAreaRadius  = mMaxVoronoiAreaSize->getValue();

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

  mSettings.objectFilter.excludeAreasAtEdges             = mExcludeAreasAtTheEdges->getValue();
  mSettings.objectFilter.excludeAreasWithoutCenterOfMass = mExcludeAreasWithoutCenterOfMass->getValue();

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