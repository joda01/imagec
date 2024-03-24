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
#include "backend/settings/analze_settings_parser.hpp"
#include "backend/settings/channel_settings.hpp"
#include "backend/settings/pipeline_settings.hpp"
#include "ui/container_base.hpp"
#include "ui/container_function.hpp"
#include <nlohmann/json_fwd.hpp>
#include "panel_voronoi_edit.hpp"
#include "panel_voronoi_overview.hpp"

namespace joda::ui::qt {

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
  ContainerVoronoi(WindowMain *windowMain);
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

  void fromJson(std::optional<joda::settings::json::ChannelSettings>,
                std::optional<joda::settings::json::PipelineStepVoronoi>) override;
  [[nodiscard]] ConvertedChannels toJson() const override;

  std::tuple<std::string, float> getMinColocFactor() override
  {
    if(mColocGroup->hasValue()) {
      return {mColocGroup->getValue().toStdString(), mColocGroup->getValueSecond() / 100.0F};
    }
    return {"NONE", 0};
  }

  void setMinColocFactor(const std::string &group, float newValue) override
  {
    if(group == mColocGroup->getValue().toStdString()) {
      mColocGroup->setValueSecond(newValue * 100.0F);
    }
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<ContainerFunction<QString, QString>> mChannelName;
  std::shared_ptr<ContainerFunction<QString, int>> mChannelColor;

  // Cell approximation//////////////////////////////////
  std::shared_ptr<ContainerFunction<int, int>> mVoronoiPoints;
  std::shared_ptr<ContainerFunction<int, int>> mMaxVoronoiAreaSize;
  std::shared_ptr<ContainerFunction<int, int>> mOverlayMaskChannelIndex;

  // Cross-Channel//////////////////////////////////
  std::shared_ptr<ContainerFunction<QString, int>> mColocGroup;
  std::shared_ptr<ContainerFunction<QString, int>> mCrossChannelIntensity;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PanelVoronoiOverview *mPanelOverview;
  PanelVoronoiEdit *mPanelEdit;
};

}    // namespace joda::ui::qt
