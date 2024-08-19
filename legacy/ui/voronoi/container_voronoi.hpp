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
#include "../container_base.hpp"
#include "../container_function.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/vchannel/vchannel_voronoi_settings.hpp"
#include <nlohmann/json_fwd.hpp>
#include "panel_voronoi_edit.hpp"
#include "panel_voronoi_overview.hpp"

namespace joda::ui {

///
/// \class      ContainerVoronoi
/// \author     Joachim Danmayr
/// \brief
///
class ContainerVoronoi : public ContainerBase
{
  friend class PanelVoronoiOverview;
  friend class PanelVoronoiEdit;

public:
  ContainerVoronoi(const ContainerVoronoi &)            = default;
  ContainerVoronoi(ContainerVoronoi &&)                 = delete;
  ContainerVoronoi &operator=(const ContainerVoronoi &) = default;
  ContainerVoronoi &operator=(ContainerVoronoi &&)      = delete;
  /////////////////////////////////////////////////////
  ContainerVoronoi(WindowMain *windowMain, joda::settings::VChannelVoronoi &settings);
  ~ContainerVoronoi();
  PanelVoronoiOverview *getOverviewPanel() override
  {
    return mPanelOverview;
  }
  PanelVoronoiEdit *getEditPanel() override
  {
    return mPanelEdit;
  }
  void setActive(bool setActive) override
  {
    if(mPanelEdit != nullptr) {
      mPanelEdit->setActive(setActive);
    }
  }

  void fromSettings(const joda::settings::VChannelVoronoi &settings);
  void toSettings() override;
  nlohmann::json toJson(const std::string &titlePrefix = "") override;

private:
  /////////////////////////////////////////////////////
  joda::settings::VChannelVoronoi &mSettings;

  /////////////////////////////////////////////////////
  std::shared_ptr<Setting<QString, QString>> mChannelName;
  std::shared_ptr<Setting<joda::settings::ChannelSettingsMeta::Type, QString>> mChannelType;
  std::shared_ptr<Setting<QString, joda::settings::ChannelIndex>> mColorAndChannelIndex;

  // Cell approximation//////////////////////////////////
  std::shared_ptr<Setting<joda::settings::ChannelIndex, int>> mVoronoiPoints;
  std::shared_ptr<Setting<int, int>> mMaxVoronoiAreaSize;
  std::shared_ptr<Setting<joda::settings::ChannelIndex, int>> mOverlayMaskChannelIndex;

  // Filtering//////////////////////////////////
  std::shared_ptr<Setting<int, int>> mMinParticleSize;
  std::shared_ptr<Setting<int, int>> mMaxParticleSize;

  std::shared_ptr<Setting<bool, bool>> mExcludeAreasWithoutCenterOfMass;
  std::shared_ptr<Setting<bool, bool>> mExcludeAreasAtTheEdges;

  // Cross-Channel//////////////////////////////////
  std::shared_ptr<Setting<QString, int>> mCrossChannelIntensity;
  std::shared_ptr<Setting<QString, int>> mCrossChannelCount;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PanelVoronoiOverview *mPanelOverview;
  PanelVoronoiEdit *mPanelEdit;
};

}    // namespace joda::ui
