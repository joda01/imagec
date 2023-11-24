#include "frame_main_controller.h"
#include <wx/filedlg.h>
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include <variant>
#include "backend/settings/channel_settings.hpp"
#include "ui/wxwidgets/dialog_processing_controller.h"
#include "ui/wxwidgets/panel_cell_approx_controller.h"
#include "ui/wxwidgets/wxwidget.h"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::wxwidget {

FrameMainController::FrameMainController(wxWindow *parent, joda::ctrl::Controller *pipelineController) :
    frameMain(parent), mPipelineController(pipelineController)
{
  mRefreshTimer = std::make_shared<std::thread>(&FrameMainController::refreshFunction, this);
  mButtonSettings->Enable(false);
  addChannel();
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
    wxString newTextDir;
    if(mPipelineController->getNrOfFoundImages() > 0) {
      mToolBar->EnableTool(mButtonRun->GetId(), true);
      newTextDir = wxString::Format("Images dir (%d):", mPipelineController->getNrOfFoundImages());

    } else {
      mToolBar->EnableTool(mButtonRun->GetId(), false);
      newTextDir = wxString::Format("Images dir:");
    }

    CallAfter([this, newTextDir]() { mLabelNrOfFoundFiles->SetLabel(newTextDir); });
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

///
/// \brief      Adds a new channel
/// \author     Joachim Danmayr
/// \return
///
void FrameMainController::addChannel()
{
  auto channel = std::make_shared<PanelChannelController>(this, mScrollbarChannels, wxID_ANY, wxDefaultPosition,
                                                          wxDefaultSize, wxTAB_TRAVERSAL);
  mSizerChannels->Insert(mChannels.size(), channel.get(), 0, wxEXPAND | wxALL, 5);
  mScrollbarChannels->Layout();
  mSizerChannels->Layout();
  mSizerChannelsScrollbar->Layout();
  this->Layout();
  mChannels.push_back(channel);
}

///
/// \brief      Remove a channel
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::removeChannel(int32_t channelIndex)
{
  mSizerChannels->Remove(channelIndex);
  auto it = mChannels.begin() + channelIndex;
  mChannels.erase(it);
  mScrollbarChannels->Layout();
  mSizerChannels->Layout();
  mSizerChannelsScrollbar->Layout();
  this->Layout();
}

void FrameMainController::removeChannel(void *toRemove)
{
  for(int n = 0; n < mChannels.size(); n++) {
    if(mChannels[n].get() == toRemove) {
      removeChannel(n);
      break;
    }
  }
}

///
/// \brief      Remove all channel
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::removeAllChannels()
{
  for(int n = mChannels.size() - 1; n >= 0; n--) {
    removeChannel(n);
  }
}

///
/// \brief      Adds a new pipelinestep
/// \author     Joachim Danmayr
/// \return
///
void FrameMainController::addPipelineStep()
{
  auto intersectionStep = std::make_shared<PanelIntersectionControl>(this, mScrrollbarPipelineStep, wxID_ANY,
                                                                     wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  mSizerPipelineStep->Insert(mPipelineStep.size() + PIPELINE_STEP_PANEL_INDEX_OFFSET, intersectionStep.get(), 0,
                             wxEXPAND | wxALL, 5);
  mScrrollbarPipelineStep->Layout();
  mSizerPipelineStep->Layout();
  mSizerHorizontalScrolPipelineSteps->Layout();
  this->Layout();
  mPipelineStep.push_back(intersectionStep);
}

///
/// \brief      Adds a new pipelinestep
/// \author     Joachim Danmayr
/// \return
///
void FrameMainController::addPipelineStepCellApprox()
{
  auto cellApproxStep = std::make_shared<PanelCellApproxController>(this, mScrrollbarPipelineStep, wxID_ANY,
                                                                    wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  mSizerPipelineStep->Insert(mPipelineStep.size() + PIPELINE_STEP_PANEL_INDEX_OFFSET, cellApproxStep.get(), 0,
                             wxEXPAND | wxALL, 5);
  mScrrollbarPipelineStep->Layout();
  mSizerPipelineStep->Layout();
  mSizerHorizontalScrolPipelineSteps->Layout();
  this->Layout();
  mPipelineStep.push_back(cellApproxStep);
}

///
/// \brief      Removes a pipelinestep
/// \author     Joachim Danmayr
/// \return
///
void FrameMainController::removePipelineStep(int32_t pipelineStepIndex)
{
  mSizerPipelineStep->Remove(pipelineStepIndex + PIPELINE_STEP_PANEL_INDEX_OFFSET);
  auto it = mPipelineStep.begin() + pipelineStepIndex;
  mPipelineStep.erase(it);
  mScrrollbarPipelineStep->Layout();
  mSizerPipelineStep->Layout();
  mSizerHorizontalScrolPipelineSteps->Layout();
  this->Layout();
}

void FrameMainController::removePipelineStep(void *toRemove)
{
  for(int n = 0; n < mPipelineStep.size(); n++) {
    if(mPipelineStep[n].get() == toRemove) {
      removePipelineStep(n);
      break;
    }
  }
}

///
/// \brief      Removes all pipelinestep
/// \author     Joachim Danmayr
/// \return
///
void FrameMainController::removeAllPipelineSteps()
{
  for(int n = mPipelineStep.size() - 1; n >= 0; n--) {
    removePipelineStep(n);
  }
}

///
/// \brief      Open settings clicked
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onOpenSettingsClicked(wxCommandEvent &event)
{
  wxFileDialog openFileDialog(this, "Open File", mLastOpenedFolder, "", "JSON files (*.json)|*.json",
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  if(openFileDialog.ShowModal() == wxID_CANCEL) {
    return;    // User cancelled the dialog
  }

  joda::settings::json::AnalyzeSettings settings;
  std::string path = openFileDialog.GetPath().ToStdString();
  settings.loadConfigFromFile(path);
  loadValues(settings);
  mLastOpenedFolder = path;
}

///
/// \brief      Save settings clicked
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onSaveSettingsClicked(wxCommandEvent &event)
{
  wxFileDialog saveFileDialog(this, "Save File", mLastOpenedFolder, "", "JSON files (*.json)|*.json", wxFD_SAVE);

  if(saveFileDialog.ShowModal() == wxID_CANCEL) {
    return;    // User cancelled the dialog
  }

  joda::settings::json::AnalyzeSettings settings;
  settings.loadConfigFromString(getValues().dump());
  std::string path = saveFileDialog.GetPath().ToStdString();
  if(!path.ends_with(".json")) {
    path += ".json";
  }
  settings.storeConfigToFile(path);
  mLastOpenedFolder = path;
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
/// \brief      Add pipeline step clicked
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onAddIntersectionClicked(wxCommandEvent &event)
{
  addPipelineStep();
}

///
/// \brief      Add cell approx clicked
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onAddCellApproxClicked(wxCommandEvent &event)
{
  addPipelineStepCellApprox();
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

  try {
    settings.loadConfigFromString(jsonStr);
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }

  try {
    DialogProcessingController dialog(this, mPipelineController, &settings);
    dialog.ShowModal();
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
    showErrorDialog(ex.what());
  }
}

///
/// \brief      Working directory changed
/// \author     Joachim Danmayr
/// \param[in]  event
///
void FrameMainController::onWorkingDirChanged(wxFileDirPickerEvent &event)
{
  std::string path = mDirectoryPicker->GetPath().ToStdString();
  mPipelineController->setWorkingDirectory(path);
  mLastOpenedFolder = path;
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
/// \brief      Assigns analyze settings to the UI components
/// \author     Joachim Danmayr
///
void FrameMainController::loadValues(const joda::settings::json::AnalyzeSettings &settings)
{
  removeAllChannels();
  removeAllPipelineSteps();

  for(const auto &channel : settings.getChannelsVector()) {
    addChannel();
    std::shared_ptr<PanelChannelController> channelUi = mChannels.back();
    channelUi->loadValues(channel);
  }

  // mDirectoryPicker->SetPath(const wxString &str);
  for(const auto &pipelineStep : settings.getPipelineSteps()) {
    if(pipelineStep.getCellApproximation()) {
      addPipelineStepCellApprox();
      auto intersectUi = mPipelineStep.back();
      intersectUi->loadValues(pipelineStep);
    }

    if(pipelineStep.getIntersection()) {
      addPipelineStep();
      auto intersectUi = mPipelineStep.back();
      intersectUi->loadValues(pipelineStep);
    }
  }

  // mChoiceNucluesChannel->SetSelection(settings.getPipelineSteps());
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

  for(const auto &intersect : mPipelineStep) {
    auto data = intersect->getValues();
    if(!data.empty()) {
      pipelineStepArray.push_back(data);
    }
  }

  jsonSettings["pipeline_steps"] = pipelineStepArray;

  jsonSettings["options"]["pixel_in_micrometer"]  = 1;
  jsonSettings["options"]["with_control_images"]  = true;
  jsonSettings["options"]["with_detailed_report"] = true;

  return jsonSettings;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void FrameMainController::showErrorDialog(const std::string &what)
{
  wxMessageBox(what, "Error", wxOK | wxICON_ERROR, this);
}

}    // namespace joda::ui::wxwidget
