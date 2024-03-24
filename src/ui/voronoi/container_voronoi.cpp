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
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/settings/channel_settings.hpp"
#include "ui/container_function.hpp"
#include "ui/voronoi/panel_voronoi_edit.hpp"
#include "ui/window_main.hpp"
#include "panel_voronoi_overview.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerVoronoi::ContainerVoronoi(WindowMain *windowMain) : mWindowMain(windowMain)
{
  mChannelName = std::shared_ptr<ContainerFunction<QString, QString>>(
      new ContainerFunction<QString, QString>("icons8-text-50.png", "Name", "Channel Name", "Voronoi"));

  mColorAndChannelIndex = std::shared_ptr<ContainerFunction<QString, int>>(
      new ContainerFunction<QString, int>("icons8-unknown-status-50.png", "Type", "Channel index", "", "#B91717",
                                          {{"#B91717", "", "icons8-bubble-50red-#B91717.png"},
                                           {"#06880C", "", "icons8-bubble-50 -green-#06880C.png"},
                                           {"#1771B9", "", "icons8-bubble-blue-#1771B9-50.png"},
                                           {"#FBEA25", "", "icons8-bubble-50-yellow-#FBEA25.png"},
                                           {"#6F03A6", "", "icons8-bubble-50-violet-#6F03A6.png"},
                                           {"#818181", "", "icons8-bubble-50-gray-#818181.png"},
                                           /*{"#000000", "", "icons8-bubble-50-black-#000000.png"}*/},
                                          {{100, "Channel 100"}, {101, "Channel 101"}}, 100));

  auto foundAIModels = joda::onnx::Onnx::findOnnxFiles();
  std::vector<ContainerFunction<QString, QString>::ComboEntry> aiModelsConverted;
  aiModelsConverted.reserve(foundAIModels.size());
  for(const auto &[path, model] : foundAIModels) {
    aiModelsConverted.push_back(
        ContainerFunction<QString, QString>::ComboEntry{.key = path.data(), .label = path.data()});
  }

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

  mCrossChannelCount = std::shared_ptr<ContainerFunction<QString, int>>(
      new ContainerFunction<QString, int>("icons8-3-50.png", "[A,B,0,1,2,3,..]", "Cross channel count", ""));

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
void ContainerVoronoi::fromJson(std::optional<joda::settings::json::ChannelSettings>,
                                std::optional<joda::settings::json::PipelineStepVoronoi> voronoi)
{
  if(voronoi.has_value()) {
    mMaxVoronoiAreaSize->clearValue();
    mColocGroup->clearValue();
    mCrossChannelIntensity->clearValue();

    // Meta
    mChannelName->setValue(voronoi->getName().data());
    mColorAndChannelIndex->setValue(QString(voronoi->getColor().data()));
    mColorAndChannelIndex->setValueSecond(voronoi->getChannelIndex());
    mVoronoiPoints->setValue(voronoi->getPointsChannelIndex());

    // Filtering
    mMaxVoronoiAreaSize->setValue(voronoi->getMaxVoronoiAreaRadius());
    mOverlayMaskChannelIndex->setValue(voronoi->getOverlayMaskChannelIndex());

    //
    // Cross channel
    //
    // Coloc
    {
      auto &coloc = voronoi->getColocGroups();
      if(coloc.size() > 0) {
        std::string value = *coloc.begin();
        mColocGroup->setValue(value.data());
        mColocGroup->setValueSecond(static_cast<int>(voronoi->getMinColocArea() * 100.0F));
      }
    }

    // Cross channel intensity
    {
      auto &crossChannelIntensityChannels = voronoi->getCrossChannelIntensityChannels();
      QString crossChannelIndexes;
      for(const auto chIdx : crossChannelIntensityChannels) {
        crossChannelIndexes += QString::number(chIdx) + ",";
      }
      if(crossChannelIndexes.size() > 0) {
        crossChannelIndexes.remove(crossChannelIndexes.lastIndexOf(","), 1);
      }
      mCrossChannelIntensity->setValue(crossChannelIndexes);
    }

    // Cross channel count
    {
      auto &crosschannelCount = voronoi->getCrossChannelCountChannels();
      QString crossChannelIndexes;
      for(const auto chIdx : crosschannelCount) {
        crossChannelIndexes += QString(chIdx.data()) + ",";
      }
      if(crossChannelIndexes.size() > 0) {
        crossChannelIndexes.remove(crossChannelIndexes.lastIndexOf(","), 1);
      }
      mCrossChannelCount->setValue(crossChannelIndexes);
    }
  }
}

///
/// \brief      Get values
/// \author     Joachim Danmayr
///
ContainerVoronoi::ConvertedChannels ContainerVoronoi::toJson() const
{
  nlohmann::json chSettings;

  chSettings["voronoi"]["name"]                       = mChannelName->getValue().toStdString();
  chSettings["voronoi"]["index"]                      = mColorAndChannelIndex->getValueSecond();
  chSettings["voronoi"]["color"]                      = mColorAndChannelIndex->getValue().toStdString();
  chSettings["voronoi"]["points_channel_index"]       = mVoronoiPoints->getValue();
  chSettings["voronoi"]["overlay_mask_channel_index"] = mOverlayMaskChannelIndex->getValue();
  chSettings["voronoi"]["max_voronoi_area_radius"]    = mMaxVoronoiAreaSize->getValue();

  // Cross channel settings
  {
    std::set<std::string> colocGroup;
    if(mColocGroup->hasValue()) {
      colocGroup.emplace(mColocGroup->getValue().toStdString());
    }
    chSettings["voronoi"]["coloc_groups"]   = colocGroup;
    chSettings["voronoi"]["min_coloc_area"] = static_cast<float>(mColocGroup->getValueSecond()) / 100.0F;
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
    chSettings["voronoi"]["cross_channel_intensity_channels"] = crossChannelIntensity;
  }

  {
    std::set<std::string> crossChannelCount;
    auto values = mCrossChannelCount->getValue().split(",");
    for(const auto &val : values) {
      crossChannelCount.emplace(val.toStdString());
    }
    chSettings["voronoi"]["cross_channel_count_channels"] = crossChannelCount;
  }

  return {.channelSettings = std::nullopt, .pipelineStepVoronoi = chSettings};
}

}    // namespace joda::ui::qt
