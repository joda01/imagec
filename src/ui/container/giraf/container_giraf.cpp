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

#include "container_giraf.hpp"
#include <qwidget.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "../../window_main.hpp"
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
#include <nlohmann/json_fwd.hpp>
#include "panel_giraf_edit.hpp"
#include "panel_giraf_overview.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerGiraf::ContainerGiraf(WindowMain *windowMain) : mWindowMain(windowMain)
{
  mPanelEdit = new PanelGirafEdit(windowMain, this);

  mPanelEdit->init();
  //
  // Create panels -> Must be after creating the functions
  //
  mPanelOverview = new PanelGirafOverview(windowMain, this);
}

///
/// \brief      Load values
/// \author     Joachim Danmayr
///
ContainerGiraf::~ContainerGiraf()
{
  delete mPanelOverview;
  delete mPanelEdit;
}

///
/// \brief      Load values
/// \author     Joachim Danmayr
///
void ContainerGiraf::fromSettings()
{
}

///
/// \brief      Get values
/// \author     Joachim Danmayr
///
void ContainerGiraf::toSettings()
{
}

}    // namespace joda::ui::qt
