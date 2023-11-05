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
#include <tiffconf.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/mstream.h>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include "backend/helper/two_way_map.hpp"
#include "backend/settings/channel_settings.hpp"
#include "ui/wxwidgets/dialog_image_controller.h"
#include "ui/wxwidgets/frame_main_controller.h"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::wxwidget {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelChannelController::PanelChannelController(FrameMainController *mainFrame, wxWindow *parent, wxWindowID id,
                                               const wxPoint &pos, const wxSize &size, long style,
                                               const wxString &name) :
    PanelChannel(parent, id, pos, size, style, name),
    mMainFrame(mainFrame)
{
  mPreviewRefreshThread = std::make_shared<std::thread>(&PanelChannelController::refreshPreviewThread, this);
}

///
/// \brief      Destructor
/// \author     Joachim Danmayr
///
PanelChannelController::~PanelChannelController()
{
  mStopped = true;
  mPreviewRefreshThread->join();
}

///
/// \brief      On remove clicked
/// \author     Joachim Danmayr
/// \return
///
void PanelChannelController::onRemoveClicked(wxCommandEvent &event)
{
  mMainFrame->removeChannel(this);
}

///
/// \brief      On preview clicked
/// \author     Joachim Danmayr
/// \return
///
void PanelChannelController::onPreviewClicked(wxCommandEvent &event)
{
  settings::json::ChannelSettings chs;
  chs.loadConfigFromString(getValues().dump());

  wxWindowID winId = mPreviewDialogs.size();
  if(!mPreviewDialogs.contains(0)) {
    // There must be always onw window with zero.
    winId = 0;
  }
  std::string title = "Channel: " + std::to_string(chs.getChannelInfo().getChannelIndex()) + " (" +
                      chs.getChannelInfo().getName() + ")";

  if(winId == 0) {
    title += " LIVE";
  } else {
    title += " Snapshot";
  }

  auto imgDialog = std::make_shared<DialogImageController>(this, winId, title);
  mPreviewDialogs.emplace(winId, imgDialog);
  imgDialog->Bind(wxEVT_CLOSE_WINDOW, &PanelChannelController::onPreviewDialogClosed, this);
  imgDialog->Show();
  std::thread([this, &imgDialog] { refreshPreview(imgDialog); }).detach();
}

///
/// \brief      Closed
/// \author     Joachim Danmayr
/// \return
///
void PanelChannelController::onPreviewDialogClosed(wxCloseEvent &ev)
{
  auto id = ev.GetId();
  mPreviewDialogs.erase(id);
  if(id == 0) {
    // Close all preview windows if reference window is closed
    mPreviewDialogs.clear();
  }
}

///
/// \brief      On preview clicked
/// \author     Joachim Danmayr
/// \return
///
void PanelChannelController::updatePreview()
{
  mLastPreviewUpdateRequest = std::chrono::high_resolution_clock::now();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelChannelController::refreshPreviewThread()
{
  while(!mStopped) {
    auto actTime = std::chrono::high_resolution_clock::now();

    if(mLastPreviewUpdateRequest > mLastPreviewUpdate) {
      mLastPreviewUpdate = std::chrono::high_resolution_clock::now();
      if(mPreviewDialogs.contains(0)) {
        refreshPreview(mPreviewDialogs[0]);
      }
      // Update only once per second
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelChannelController::refreshPreview(std::shared_ptr<DialogImageController> dialog)
{
  try {
    dialog->startProgress(2000);
    settings::json::ChannelSettings chs;
    chs.loadConfigFromString(getValues().dump());
    auto ret = mMainFrame->getController()->preview(chs, 0);
    wxMemoryInputStream stream(ret.data.data(), ret.data.size());
    wxImage image;
    if(!image.LoadFile(stream, wxBITMAP_TYPE_PNG)) {
      wxLogError("Failed to load PNG image from bytes.");
    } else {
      int64_t valid   = 0;
      int64_t inValid = 0;

      for(const auto &roi : ret.detectionResult) {
        if(roi.isValid()) {
          valid++;
        } else {
          inValid++;
        }
      }

      dialog->updateImage(image, {.valid = valid, .invalid = inValid});
    }
  } catch(const std::exception &ex) {
    // showErrorDialog(ex.what());
  }
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
  mMainFrame->mChoiceSeries->SetSelection(channelSettings.getChannelInfo().getChannelSeries());
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
    if(prepro.getGaussianBlur()) {
      mDropdownGausianBlur->SetSelection(
          filterKernelToIndex(static_cast<int16_t>(prepro.getGaussianBlur()->kernel_size)));
      mDropDownGausianBlurRepeat->SetSelection(prepro.getGaussianBlur()->repeat - 1);
    }
    if(prepro.getSmoothing()) {
      mDropDownSmoothingRepeat->SetSelection(prepro.getSmoothing()->repeat);
    }
    if(prepro.getMedianBgSubtraction()) {
      mChoiceMedianBGSubtract->SetSelection(1);
    }
    if(prepro.getSubtractChannel()) {
      mChoiceBGSubtraction->SetSelection(prepro.getSubtractChannel()->channel_index + 1);
    }
  }

  // Detection
  mCheckUseAI->SetValue(settings::json::ChannelDetection::DetectionMode::AI ==
                        channelSettings.getDetectionSettings().getDetectionMode());
  mChoiceThresholdMethod->SetSelection(
      thresholdToIndex(channelSettings.getDetectionSettings().getThersholdSettings().getThresholdString()));
  mSpinMinThreshold->SetValue(channelSettings.getDetectionSettings().getThersholdSettings().getThresholdMinError());

  // Filtering
  mSpinMinCircularity->SetValue(channelSettings.getFilter().getMinCircularity());
  std::string range = std::to_string(static_cast<uint32_t>(channelSettings.getFilter().getMinParticleSize())) + "-" +
                      std::to_string(static_cast<uint32_t>(channelSettings.getFilter().getMaxParticleSize()));
  mTextParticleSizeRange->SetValue(range);
  mSpinSnapArea->SetValue(channelSettings.getFilter().getSnapAreaSize());
  mChoiceReferenceSpotChannel->SetSelection(channelSettings.getFilter().getReferenceSpotChannelIndex() + 1);

  wxCommandEvent a;
  onChannelTypeChanged(a);
  onAiCheckBox(a);
}

///
/// \brief        Return the channel settings converted to a JSON object
/// \author       Joachim Danmayr
/// \return       JSON object of the settings made in the channel UI
///
nlohmann::json PanelChannelController::getValues()
{
  nlohmann::json chSettings;

  chSettings["info"]["index"]  = mChoiceChannelIndex->GetSelection();
  chSettings["info"]["series"] = mMainFrame->mChoiceSeries->GetSelection();
  chSettings["info"]["type"]   = indexToType(mChoiceChannelType->GetSelection());
  chSettings["info"]["label"]  = "";
  chSettings["info"]["name"]   = mTextChannelName->GetValue();

  // Preprocessing
  nlohmann::json jsonArray = nlohmann::json::array();    // Initialize an empty JSON array
  jsonArray.push_back({{"z_stack", {{"value", indexToZProjection(mChoiceZStack->GetSelection())}}}});
  if(mSpinMarginCrop->GetValue() > 0) {
    jsonArray.push_back({{"margin_crop", {{"value", static_cast<int>(mSpinMarginCrop->GetValue())}}}});
  }
  if(mSpinRollingBall->GetValue() > 0) {
    jsonArray.push_back({{"rolling_ball", {{"value", static_cast<int>(mSpinRollingBall->GetValue())}}}});
  }
  if(mDropdownGausianBlur->GetSelection() > 0) {
    jsonArray.push_back({{"gaussian_blur",
                          {{"kernel_size", indexToFilterKernel(mDropdownGausianBlur->GetSelection())},
                           {"repeat", mDropDownGausianBlurRepeat->GetSelection() + 1}}}});
  }
  if(mDropDownSmoothingRepeat->GetSelection() > 0) {
    jsonArray.push_back({{"smoothing", {{"repeat", mDropDownSmoothingRepeat->GetSelection()}}}});
  }
  if(mChoiceMedianBGSubtract->GetSelection() > 0) {
    jsonArray.push_back({{"median_bg_subtraction", {{"kernel_size", 3}}}});
  }
  if(mChoiceBGSubtraction->GetSelection() > 0) {
    jsonArray.push_back({{"subtract_channel", {{"channel_index", mChoiceBGSubtraction->GetSelection() - 1}}}});
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
  chSettings["detection"]["threshold"]["threshold_max"] = UINT16_MAX;

  chSettings["detection"]["ai"]["model_name"]      = "AI_MODEL_COMMON_V1";
  chSettings["detection"]["ai"]["probability_min"] = 0.8;

  // Filtering
  try {
    auto [min, max] = splitAndConvert(mTextParticleSizeRange->GetLineText(0).ToStdString(), '-');
    chSettings["filter"]["min_particle_size"] = min;
    chSettings["filter"]["max_particle_size"] = max;
  } catch(const std::exception &) {
    // Invalid input number format
    chSettings["filter"]["min_particle_size"] = 0;
    chSettings["filter"]["max_particle_size"] = 0;
  }

  chSettings["filter"]["min_circularity"]              = mSpinMinCircularity->GetValue();
  chSettings["filter"]["snap_area_size"]               = mSpinSnapArea->GetValue();
  chSettings["filter"]["reference_spot_channel_index"] = mChoiceReferenceSpotChannel->GetSelection() - 1;

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

auto PanelChannelController::indexToFilterKernel(int idx) -> int16_t
{
  return GAUSSIAN_BLUR[idx];
}
auto PanelChannelController::filterKernelToIndex(int16_t kernel) -> int
{
  return GAUSSIAN_BLUR[kernel];
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
    if(tokens[0] == "Inf.") {
      num1 = INT32_MAX;
    } else {
      num1 = std::stoi(tokens[0]);
    }
    if(tokens[1] == "Inf.") {
      num2 = INT32_MAX;
    } else {
      num2 = std::stoi(tokens[1]);
    }

  } else {
    // Handle incorrect format
    std::cerr << "Incorrect format." << std::endl;
    num1 = num2 = 0;    // Or any other appropriate action
  }

  return {num1, num2};
}

///
/// \brief      Channel type has been changed
///             Make reference spot panel invisible if not a spot channel
/// \author     Joachim Danmayr
///
void PanelChannelController::onChannelTypeChanged(wxCommandEvent &event)
{
  if(mChoiceChannelType->GetSelection() != 0) {
    panelReferenceChannel->Enable(false);
  } else {
    panelReferenceChannel->Enable(true);
  }
  updatePreview();
}

///
/// \brief      Disable threshold if AI is clicked
/// \author     Joachim Danmayr
///
void PanelChannelController::onAiCheckBox(wxCommandEvent &event)
{
  if(mCheckUseAI->GetValue()) {
    panelThresholdMethod->Enable(false);
    panelMinThreshold->Enable(false);
  } else {
    panelThresholdMethod->Enable(true);
    panelMinThreshold->Enable(true);
  }
  updatePreview();
}

///
/// \brief      Min threshold changed
/// \author     Joachim Danmayr
///
void PanelChannelController::onMinThresholdChanged(wxSpinEvent &event)
{
  updatePreview();
}
void PanelChannelController::onChannelIndexChanged(wxCommandEvent &event)
{
  updatePreview();
}

void PanelChannelController::onZStackSettingsChanged(wxCommandEvent &event)
{
  updatePreview();
}
void PanelChannelController::onMarginCropChanged(wxSpinEvent &event)
{
  updatePreview();
}
void PanelChannelController::onMedianBGSubtractChanged(wxCommandEvent &event)
{
  updatePreview();
}
void PanelChannelController::onRollingBallChanged(wxSpinEvent &event)
{
  updatePreview();
}
void PanelChannelController::onBgSubtractChanged(wxCommandEvent &event)
{
  updatePreview();
}
void PanelChannelController::onSmoothingChanged(wxCommandEvent &event)
{
  updatePreview();
}
void PanelChannelController::onGausianBlurChanged(wxCommandEvent &event)
{
  updatePreview();
}
void PanelChannelController::onGausianBlurRepeatChanged(wxCommandEvent &event)
{
  updatePreview();
}
void PanelChannelController::onThresholdMethodChanged(wxCommandEvent &event)
{
  updatePreview();
}
void PanelChannelController::onMinCircularityChanged(wxSpinDoubleEvent &event)
{
  updatePreview();
}
void PanelChannelController::onParticleSizeChanged(wxCommandEvent &event)
{
  try {
    auto [min, max]         = splitAndConvert(mTextParticleSizeRange->GetLineText(0).ToStdString(), '-');
    wxColour defaultBgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    mTextParticleSizeRange->SetBackgroundColour(defaultBgColor);
    updatePreview();
  } catch(const std::exception &) {
    // Invalid input number format
    mTextParticleSizeRange->SetBackgroundColour(wxColour(255, 0, 0));
  }
}
void PanelChannelController::onSnapAreaChanged(wxSpinEvent &event)
{
  updatePreview();
}
void PanelChannelController::onSpotRemovalChanged(wxCommandEvent &event)
{
  updatePreview();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelChannelController::showErrorDialog(const std::string &what)
{
  wxMessageBox(what, "Error", wxOK | wxICON_ERROR, this);
}

}    // namespace joda::ui::wxwidget
