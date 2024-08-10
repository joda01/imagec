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
#include "backend/settings/vchannel/vchannel_intersection.hpp"
#include "backend/settings/vchannel/vchannel_voronoi_settings.hpp"
#include <nlohmann/json_fwd.hpp>
#include "panel_intersection_edit.hpp"
#include "panel_intersection_overview.hpp"

namespace joda::ui::qt {

///
/// \class      ContainerIntersection
/// \author     Joachim Danmayr
/// \brief
///
class ContainerIntersection : public ContainerBase
{
  friend class PanelIntersectionOverview;
  friend class PanelIntersectionEdit;

public:
  ContainerIntersection(const ContainerIntersection &)            = default;
  ContainerIntersection(ContainerIntersection &&)                 = delete;
  ContainerIntersection &operator=(const ContainerIntersection &) = default;
  ContainerIntersection &operator=(ContainerIntersection &&)      = delete;
  /////////////////////////////////////////////////////
  ContainerIntersection(WindowMain *windowMain, joda::settings::VChannelIntersection &settings);
  ~ContainerIntersection();
  PanelIntersectionOverview *getOverviewPanel() override
  {
    return mPanelOverview;
  }
  PanelIntersectionEdit *getEditPanel() override
  {
    return mPanelEdit;
  }
  void setActive(bool setActive) override
  {
    if(mPanelEdit != nullptr) {
      mPanelEdit->setActive(setActive);
    }
  }

  void fromSettings() override;
  void toSettings() override;
  nlohmann::json toJson() override;

private:
  /////////////////////////////////////////////////////
  joda::settings::VChannelIntersection &mSettings;

  /////////////////////////////////////////////////////
  std::shared_ptr<ContainerFunction<QString, QString>> mChannelName;
  std::shared_ptr<ContainerFunction<joda::settings::ChannelSettingsMeta::Type, QString>> mChannelType;
  std::shared_ptr<ContainerFunction<QString, joda::settings::ChannelIndex>> mColorAndChannelIndex;

  std::shared_ptr<ContainerFunction<float, int>> mMinIntersection;

  // Cross-Channel//////////////////////////////////
  std::shared_ptr<ContainerFunction<QString, int>> mCrossChannelIntersection;
  std::shared_ptr<ContainerFunction<QString, int>> mCrossChannelIntensity;
  std::shared_ptr<ContainerFunction<QString, int>> mCrossChannelCount;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PanelIntersectionOverview *mPanelOverview;
  PanelIntersectionEdit *mPanelEdit;
};

}    // namespace joda::ui::qt
