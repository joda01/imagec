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
#include "backend/settings/channel/channel_settings_meta.hpp"
#include "backend/settings/detection/detection_settings.hpp"
#include "backend/settings/preprocessing/functions/edge_detection.hpp"
#include "backend/settings/preprocessing/functions/rolling_ball.hpp"
#include "backend/settings/preprocessing/functions/zstack.hpp"
#include <nlohmann/json_fwd.hpp>
#include "panel_giraf_edit.hpp"
#include "panel_giraf_overview.hpp"

namespace joda::ui::qt {

///
/// \class      ContainerGiraf
/// \author     Joachim Danmayr
/// \brief
///
class ContainerGiraf : public ContainerBase
{
  friend class PanelGirafOverview;
  friend class PanelGirafEdit;

public:
  ContainerGiraf(const ContainerGiraf &)            = delete;
  ContainerGiraf(ContainerGiraf &&)                 = delete;
  ContainerGiraf &operator=(const ContainerGiraf &) = delete;
  ContainerGiraf &operator=(ContainerGiraf &&)      = delete;

  /////////////////////////////////////////////////////
  ContainerGiraf(WindowMain *windowMain);
  ~ContainerGiraf() override;
  PanelGirafOverview *getOverviewPanel() override
  {
    return mPanelOverview;
  }
  PanelGirafEdit *getEditPanel() override
  {
    return mPanelEdit;
  }
  void setActive(bool setActive) override
  {
    if(mPanelEdit != nullptr) {
      mPanelEdit->setActive(setActive);
    }
  }

  void fromSettings();
  void toSettings() override;

private:
  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PanelGirafOverview *mPanelOverview;
  PanelGirafEdit *mPanelEdit;
};

}    // namespace joda::ui::qt
