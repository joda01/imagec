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
#include <string>
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
/// \brief      Initializes the UI elements with the values from the channel settings
/// \author     Joachim Danmayr
/// \param[in]  channelSettings  Channel settings
///
void PanelChannelController::loadValues(const joda::settings::json::ChannelSettings &channelSettings)
{
  // Channel Info
  mChoiceChannelIndex->SetSelection(channelSettings.getChannelInfo().getChannelIndex());
  mChoiceChannelType->SetSelection(typeToIndex(channelSettings.getChannelInfo().getTypeString()));
  mTextChannelName->SetValue(channelSettings.getChannelInfo().getName());

  // Preprocessing
  for(const auto &prepro : channelSettings.getPreprocessingFunctions()) {
    if(prepro.getZStack()) {
      mChoiceZStack->SetSelection(zProjectionToIndex(prepro.getZStack()->value));
    }
    if(prepro.getRollingBall()) {
      mSpinRollingBall->SetValue(prepro.getRollingBall()->value);
    }
    if(prepro.getMarginCrop()) {
      mSpinMarginCrop->SetValue(prepro.getMarginCrop()->value);
    }
  }

  // Detection
  mCheckUseAI->SetValue(settings::json::ChannelDetection::DetectionMode::AI ==
                        channelSettings.getDetectionSettings().getDetectionMode());
  mChoiceThresholdMethod->SetSelection(
      thresholdToIndex(channelSettings.getDetectionSettings().getThersholdSettings().getThresholdString()));
  mSpinMinThreshold->SetValue(channelSettings.getDetectionSettings().getThersholdSettings().getThresholdMin());

  // Filtering
  mSpinMinCircularity->SetValue(channelSettings.getFilter().getMinCircularity());
  std::string range = std::to_string(channelSettings.getFilter().getMinCircularity()) + "-" +
                      std::to_string(channelSettings.getFilter().getMaxParticleSize());
  mTextParticleSizeRange->SetValue(range);
  mSpinSnapArea->SetValue(channelSettings.getFilter().getSnapAreaSize());
}

///
/// \brief        Return the channel settings converted to a JSON object
/// \author       Joachim Danmayr
/// \return       JSON object of the settings made in the channel UI
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
  jsonArray.push_back({{"z_stack", {{"value", indexToZProjection(mChoiceZStack->GetSelection())}}}});
  if(mSpinMarginCrop->GetValue() > 0) {
    jsonArray.push_back({{"margin_crop", {{"value", static_cast<int>(mSpinMarginCrop->GetValue())}}}});
  }
  if(mSpinRollingBall->GetValue() > 0) {
    jsonArray.push_back({{"rolling_ball", {{"value", static_cast<int>(mSpinRollingBall->GetValue())}}}});
  }
  chSettings["preprocessing"] = jsonArray;

  // Detections
  if(mCheckUseAI->IsChecked()) {
    chSettings["detection"]["mode"] = "AI";
  } else {
    chSettings["detection"]["mode"] = "THRESHOLD";
  }

  chSettings["detection"]["threshold"]["threshold_algorithm"] =
      indexToThreshold(mChoiceThresholdMethod->GetSelection());
  chSettings["detection"]["threshold"]["threshold_min"] = static_cast<int>(mSpinMinThreshold->GetValue());
  chSettings["detection"]["threshold"]["threshold_max"] = 65535;

  chSettings["detection"]["ai"]["model_name"]      = "AI_MODEL_COMMON_V1";
  chSettings["detection"]["ai"]["probability_min"] = 0.8;

  // Filtering
  auto [min, max] = splitAndConvert(mTextParticleSizeRange->GetLineText(0).ToStdString(), '-');
  chSettings["filter"]["min_particle_size"] = min;
  chSettings["filter"]["max_particle_size"] = max;
  chSettings["filter"]["min_circularity"]   = mSpinMinCircularity->GetValue();
  chSettings["filter"]["snap_area_size"]    = mSpinSnapArea->GetValue();

  return chSettings;
}

auto PanelChannelController::indexToType(int idx) -> std::string
{
  return CHANNEL_TYPES[idx];
}

auto PanelChannelController::typeToIndex(const std::string &str) -> int
{
  return CHANNEL_TYPES[str];
}

auto PanelChannelController::indexToZProjection(int idx) -> std::string
{
  return Z_STACK_PROJECTION[idx];
}

auto PanelChannelController::zProjectionToIndex(const std::string &str) -> int
{
  return Z_STACK_PROJECTION[str];
}

auto PanelChannelController::indexToThreshold(int idx) -> std::string
{
  return THRESHOLD_METHOD[idx];
}

auto PanelChannelController::thresholdToIndex(const std::string &str) -> int
{
  return THRESHOLD_METHOD[str];
}

auto PanelChannelController::splitAndConvert(const std::string &input, char delimiter) -> std::tuple<int, int>
{
  std::istringstream ss(input);
  std::string token;
  std::vector<std::string> tokens;

  while(std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }
  int num1 = 0;
  int num2 = 0;
  if(tokens.size() == 2) {
    num1 = std::stoi(tokens[0]);
    num2 = std::stoi(tokens[1]);
  } else {
    // Handle incorrect format
    std::cerr << "Incorrect format." << std::endl;
    num1 = num2 = 0;    // Or any other appropriate action
  }

  return {num1, num2};
}

}    // namespace joda::ui::wxwidget
