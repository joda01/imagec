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

#include "container_intersection.hpp"
#include <qwidget.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "../../window_main.hpp"
#include "../container_function.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "panel_intersection_edit.hpp"
#include "panel_intersection_overview.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerIntersection::ContainerIntersection(WindowMain *windowMain, joda::settings::VChannelIntersection &settings) :
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
          joda::settings::ChannelIndex::A));

  mMinIntersection = std::shared_ptr<ContainerFunction<float, int>>(
      new ContainerFunction<float, int>("icons8-percentage-50.png", "[0 - 1]", "Min. intersection", "%", 0.1, 0, 1));

  mCrossChannelIntersection = std::shared_ptr<ContainerFunction<QString, int>>(new ContainerFunction<QString, int>(
      "icons8-query-inner-join-50.png", "[A,B,C,0,1,2,3,..]", "Cross channel intersection", ""));

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
  mPanelOverview = new PanelIntersectionOverview(windowMain, this);
  mPanelEdit     = new PanelIntersectionEdit(windowMain, this);
}

///
/// \brief      Load values
/// \author     Joachim Danmayr
///
ContainerIntersection::~ContainerIntersection()
{
  delete mPanelOverview;
  delete mPanelEdit;
}

///
/// \brief      Load values
/// \author     Joachim Danmayr
///
void ContainerIntersection::fromSettings()
{
  mCrossChannelIntersection->clearValue();
  mCrossChannelCount->clearValue();
  mCrossChannelIntensity->clearValue();

  // Meta
  mChannelType->setValue(mSettings.meta.type);
  mChannelName->setValue(mSettings.meta.name.data());
  mColorAndChannelIndex->setValue(mSettings.meta.color.data());
  mColorAndChannelIndex->setValueSecond(mSettings.meta.channelIdx);

  // Filtering
  mMinIntersection->setValue(mSettings.intersection.minIntersection);

  {
    auto &crossChannelIntensity = mSettings.intersection.intersectingChannels;
    QString crossChannelIndexes;
    for(const auto chIdx : crossChannelIntensity) {
      if(static_cast<int32_t>(chIdx) < 65) {
        crossChannelIndexes += QString::number(static_cast<int32_t>(chIdx)) + ",";
      } else {
        char ch = static_cast<char>(chIdx);
        crossChannelIndexes += QString(std::to_string(ch).data()) + ",";
      }
    }
    mCrossChannelIntersection->setValue(crossChannelIndexes);
  }

  //
  // Cross channel
  //
  // Coloc

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
    mCrossChannelIntensity->setValue(crossChannelIndexes);
  }

  // Cross channel count
  {
    auto &crosschannelCount = mSettings.crossChannel.crossChannelCoutChannels;
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
void ContainerIntersection::toSettings()
{
  mSettings.meta.channelIdx = mColorAndChannelIndex->getValueSecond();
  mSettings.meta.color      = mColorAndChannelIndex->getValue().toStdString();
  mSettings.meta.series     = mWindowMain->getSelectedSeries();
  mSettings.meta.type       = mChannelType->getValue();
  mSettings.meta.name       = mChannelName->getValue().toStdString();

  mSettings.intersection.minIntersection = mMinIntersection->getValue();

  // Cross channel settings
  {
    std::set<joda::settings::ChannelIndex> interscintChannls;
    auto values = mCrossChannelIntersection->getValue().split(",");
    for(const auto &val : values) {
      if(!val.isEmpty()) {
        if(val[0] < 'A') {
          interscintChannls.emplace((joda::settings::ChannelIndex) val.toInt());
        } else {
          interscintChannls.emplace((joda::settings::ChannelIndex) val[0].toLatin1());
        }
      }
    }
    mSettings.intersection.intersectingChannels = interscintChannls;
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
    mSettings.crossChannel.crossChannelCoutChannels = crossChannelCount;
  }
}

}    // namespace joda::ui::qt
