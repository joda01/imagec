#include "frame_main_controller.h"
#include <exception>
#include <memory>
#include "backend/settings/channel_settings.hpp"
#include "ui/wxwidgets/dialog_processing_controller.h"
#include "ui/wxwidgets/wxwidget.h"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::wxwidget {

FrameMainController::FrameMainController(wxWindow *parent) : frameMain(parent)
{
  addChannel();
}

void FrameMainController::addChannel()
{
  auto channel = std::make_shared<PanelChannelController>(mScrollbarChannels, wxID_ANY, wxDefaultPosition,
                                                          wxDefaultSize, wxTAB_TRAVERSAL);
  mSizerChannels->Insert(mChannels.size(), channel.get(), 0, wxEXPAND | wxALL, 5);
  mScrollbarChannels->Layout();
  mSizerChannels->Layout();
  mSizerChannelsScrollbar->Layout();
  this->Layout();
  mChannels.push_back(channel);
}

void FrameMainController::removeChannel()
{
}

///
/// \brief      Add channel button clicked
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onAddChannelClicked(wxCommandEvent &event)
{
  addChannel();
}

///
/// \brief      Run button clicked in the toolbar
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onRunClicked(wxCommandEvent &event)
{
  nlohmann::json jsonSettings;

  nlohmann::json channelsArray = nlohmann::json::array();    // Initialize an empty JSON array
  for(const auto &ch : mChannels) {
    channelsArray.push_back(ch->getValues());
  }
  jsonSettings["input_folder"] = static_cast<std::string>(mDirectoryPicker->GetPath().ToStdString());
  jsonSettings["channels"]     = channelsArray;

  nlohmann::json pipelineStepArray = nlohmann::json::array();    // Initialize an empty JSON array
  jsonSettings["pipeline"]         = pipelineStepArray;

  jsonSettings["options"]["pixel_in_micrometer"]  = 1;
  jsonSettings["options"]["with_control_images"]  = true;
  jsonSettings["options"]["with_detailed_report"] = true;

  settings::json::AnalyzeSettings settings;
  std::string jsonStr = jsonSettings.dump();
  std::cout << jsonStr << std::endl;

  try {
    settings.loadConfigFromString(jsonStr);
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }

  DialogProcessingController dialog(this, settings);
  dialog.ShowModal();
}

///
/// \brief      About clicked
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onAboutClicked(wxCommandEvent &event)
{
  DialogAbout about(this);
  about.ShowModal();
}

}    // namespace joda::ui::wxwidget
