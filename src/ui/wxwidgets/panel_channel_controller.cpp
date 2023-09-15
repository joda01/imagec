///
/// \file      panel_channel_controller.cpp
/// \author    Joachim Danmayr
/// \date      2023-09-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "panel_channel_controller.h"
#include "backend/helper/two_way_map.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::wxwidget {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelChannelController::PanelChannelController(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size,
                                               long style, const wxString &name) :
    PanelChannel(parent, id, pos, size, style, name)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelChannelController::loadValues(const joda::settings::json::ChannelSettings &channelSettings)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
nlohmann::json PanelChannelController::getValues()
{
  nlohmann::json chSettings;

  chSettings["info"]["index"] = mChoiceChannelIndex->GetSelection();
  chSettings["info"]["type"]  = indexToType(mChoiceChannelType->GetSelection());
  chSettings["info"]["label"] = "";
  chSettings["info"]["name"]  = mTextChannelName->GetValue();

  // Preprocessing
  nlohmann::json jsonArray = nlohmann::json::array();    // Initialize an empty JSON array
  jsonArray.push_back({{"z_stack", {"value", 30}}});
  jsonArray.push_back({{"margin_crop", {"value", static_cast<int>(mSpinMarginCrop->GetValue())}}});
  jsonArray.push_back({{"rolling_ball", {"value", static_cast<int>(mSpinRollingBall->GetValue())}}});
  chSettings["preprocessing"] = jsonArray;

  // Detections
  chSettings["detection"]["mode"] = "THRESHOLD";

  chSettings["detection"]["threshold"]["threshold_algorithm"] = "";
  chSettings["detection"]["threshold"]["threshold_min"]       = static_cast<int>(mSpinMinThreshold->GetValue());
  chSettings["detection"]["threshold"]["threshold_max"]       = 65535;

  chSettings["detection"]["ai"]["model_name"]      = "AI_MODEL_COMMON_V1";
  chSettings["detection"]["ai"]["probability_min"] = 0.8;

  // Filtering
  chSettings["filter"]["min_particle_size"] = 0;
  chSettings["filter"]["max_particle_size"] = 0;
  chSettings["filter"]["min_circularity"]   = 0;
  chSettings["filter"]["snap_area_size"]    = 0;

  return chSettings;
}

auto PanelChannelController::indexToType(int idx) -> std::string
{
  return myMap[idx];
}

auto PanelChannelController::typeToIndex(const std::string &str) -> int
{
  return myMap[str];
}

}    // namespace joda::ui::wxwidget
