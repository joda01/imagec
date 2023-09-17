#include "frame_main_controller.h"
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include "backend/settings/channel_settings.hpp"
#include "ui/wxwidgets/dialog_processing_controller.h"
#include "ui/wxwidgets/wxwidget.h"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::wxwidget {

FrameMainController::FrameMainController(wxWindow *parent, joda::ctrl::Controller *pipelineController) :
    frameMain(parent), mPipelineController(pipelineController)
{
  mRefreshTimer = std::make_shared<std::thread>(&FrameMainController::refreshFunction, this);
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

FrameMainController::~FrameMainController()
{
  mStopped = true;
  mRefreshTimer->join();
}

///
/// \brief      Updates dynamic information
/// \author     Joachim Danmayr
///
void FrameMainController::refreshFunction()
{
  while(!mStopped) {
    if(mPipelineController->getNrOfFoundImages() > 0) {
      mLabelNrOfFoundFiles->SetLabel(_("Images dir ") + "(" +
                                     std::to_string(mPipelineController->getNrOfFoundImages()) + "):");
    } else {
      mLabelNrOfFoundFiles->SetLabel(_("Images dir: "));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
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
  auto jsonSettings = getValues();
  settings::json::AnalyzeSettings settings;
  std::string jsonStr = jsonSettings.dump();
  std::cout << jsonStr << std::endl;

  try {
    settings.loadConfigFromString(jsonStr);
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }

  DialogProcessingController dialog(this, mPipelineController, &settings);
  dialog.ShowModal();
}

///
/// \brief      Working directory changed
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onWorkingDirChanged(wxFileDirPickerEvent &event)
{
  mPipelineController->setWorkingDirectory(mDirectoryPicker->GetPath().ToStdString());
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

///
/// \brief      Cell channel choice
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onCellChannelChoice(wxCommandEvent &event)
{
  if(mChoiceCellChannel->GetSelection() > 0) {
    panelMaxCellRadius->Show(false);
  } else {
    panelMaxCellRadius->Show(true);
  }
}

///
/// \brief      Assigns analyze settings to the UI components
/// \author     Joachim Danmayr
///
void FrameMainController::loadValues(const joda::settings::json::AnalyzeSettings &)
{
}

///
/// \brief      Converts the UI settings to a JSON object
/// \author     Joachim Danmayr
///
nlohmann::json FrameMainController::getValues()
{
  nlohmann::json jsonSettings;

  nlohmann::json channelsArray = nlohmann::json::array();    // Initialize an empty JSON array
  for(const auto &ch : mChannels) {
    channelsArray.push_back(ch->getValues());
  }
  jsonSettings["input_folder"] = static_cast<std::string>(mDirectoryPicker->GetPath().ToStdString());
  jsonSettings["channels"]     = channelsArray;

  //
  // Pipeline steps
  //
  nlohmann::json pipelineStepArray = nlohmann::json::array();    // Initialize an empty JSON array
  if(mChoiceNucluesChannel->GetSelection() > 0 || mChoiceCellChannel->GetSelection() > 0) {
    pipelineStepArray.push_back({{"cell_approximation",
                                  {{"nucleus_channel_index", mChoiceNucluesChannel->GetSelection() - 1},
                                   {"cell_channel_index", mChoiceCellChannel->GetSelection() - 1},
                                   {"max_cell_radius", mSpinMaxCellRadius->GetValue()}}}});
  }

  std::set<int32_t> intersectionButtons;
  if(mButtonIntersectionCh01->GetValue()) {
    intersectionButtons.emplace(0);
  }
  if(mButtonIntersectionCh02->GetValue()) {
    intersectionButtons.emplace(1);
  }
  if(mButtonIntersectionCh03->GetValue()) {
    intersectionButtons.emplace(2);
  }
  if(mButtonIntersectionCh04->GetValue()) {
    intersectionButtons.emplace(3);
  }
  if(mButtonIntersectionCh05->GetValue()) {
    intersectionButtons.emplace(4);
  }
  if(mButtonIntersectionCh06->GetValue()) {
    intersectionButtons.emplace(5);
  }
  if(mButtonIntersectionCh07->GetValue()) {
    intersectionButtons.emplace(6);
  }
  if(mButtonIntersectionCh08->GetValue()) {
    intersectionButtons.emplace(7);
  }
  if(mButtonIntersectionCh09->GetValue()) {
    intersectionButtons.emplace(8);
  }
  if(mButtonIntersectionCh10->GetValue()) {
    intersectionButtons.emplace(9);
  }
  if(mButtonIntersectionCh11->GetValue()) {
    intersectionButtons.emplace(10);
  }
  if(mButtonIntersectionCh12->GetValue()) {
    intersectionButtons.emplace(11);
  }
  if(mButtonIntersectionChEstimatedCell->GetValue()) {
    intersectionButtons.emplace(
        static_cast<int32_t>(settings::json::PipelineStepSettings::PipelineStepIndex::CELL_APPROXIMATION));
  }
  if(!intersectionButtons.empty()) {
    pipelineStepArray.push_back({{"channel_index", intersectionButtons}});
  }

  jsonSettings["pipeline_steps"] = pipelineStepArray;

  jsonSettings["options"]["pixel_in_micrometer"]  = 1;
  jsonSettings["options"]["with_control_images"]  = true;
  jsonSettings["options"]["with_detailed_report"] = true;

  return jsonSettings;
}

}    // namespace joda::ui::wxwidget
