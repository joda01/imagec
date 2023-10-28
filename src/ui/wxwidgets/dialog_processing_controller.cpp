///
/// \file      dialog_processing_controller.cpp
/// \author    Joachim Danmayr
/// \date      2023-09-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "dialog_processing_controller.h"
#include <wx/wx.h>
#include <exception>
#include <string>
#include "backend/pipelines/pipeline.hpp"

namespace joda::ui::wxwidget {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
DialogProcessingController::DialogProcessingController(wxWindow *parent, joda::ctrl::Controller *pipelineController,
                                                       joda::settings::json::AnalyzeSettings *settings, wxWindowID id,
                                                       const wxString &title, const wxPoint &pos, const wxSize &size,
                                                       long style) :
    DialogProcessing(parent, id, title, pos, size, style),
    mPipelineController(pipelineController), mAnalyzeSettins(settings)
{
  mPipelineController->reset();
  refreshFunction();
  mRefreshTimer = std::make_shared<std::thread>(&DialogProcessingController::refreshThread, this);
  mPipelineController->start(*mAnalyzeSettins);
  mStartedTime = std::chrono::high_resolution_clock::now();

  //
  // Show uses RAM and threads
  //
  auto rescources = pipelineController->calcOptimalThreadNumber(*settings, 0);

  auto [ramPerImg, expoperImg] = exponentForRam(rescources.ramPerImage);
  auto [ramFree, expoFree]     = exponentForRam(rescources.ramFree);
  auto [ramTotal, expoTotal]   = exponentForRam(rescources.ramTotal);
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << ramPerImg << " " << expoperImg << " / " << std::fixed
         << std::setprecision(2) << ramFree << " " << expoFree << " / " << std::fixed << std::setprecision(2)
         << ramTotal << " " << expoTotal;
  auto ramStr = stream.str();

  std::stringstream streamCpu;
  streamCpu << std::fixed << std::setprecision(0)
            << std::to_string(rescources.cores[pipeline::Pipeline::ThreadingSettings::Type::IMAGES]) << " / "
            << std::to_string(rescources.cores[pipeline::Pipeline::ThreadingSettings::Type::TILES]) << " / "
            << std::to_string(rescources.cores[pipeline::Pipeline::ThreadingSettings::Type::CHANNELS]) << " / "
            << std::to_string(rescources.coresAvailable);
  auto cpuStr = streamCpu.str();

  CallAfter([this, ramStr, cpuStr]() {
    labelAvailableCores->SetLabel(cpuStr);
    labelRAM->SetLabel(ramStr);
  });
}

DialogProcessingController::~DialogProcessingController()
{
  mStopped = true;
  if(mRefreshTimer->joinable()) {
    mRefreshTimer->join();
  }
}

///
/// \brief      Finds the best exponent view
/// \author     Joachim Danmayr
///
std::tuple<double, std::string> DialogProcessingController::exponentForRam(double ram)
{
  if(ram > 1e9) {
    return {ram / 1e9, "GB"};
  }
  if(ram > 1e6) {
    return {ram / 1e6, "MB"};
  }
  if(ram > 1e3) {
    return {ram / 1e3, "kB"};
  }
  return {ram, "Byte"};
}

///
/// \brief      Finds the best exponent view
/// \author     Joachim Danmayr
///
std::tuple<double, std::string> DialogProcessingController::exponentForTime(double timeMs)
{
  if(timeMs > 1e3 * 60) {
    return {timeMs / (1e3 * 60), " min"};
  }

  if(timeMs > 1e3) {
    return {timeMs / 1e3, " s"};
  }
  return {timeMs, " ms"};
}

///
/// \brief      Updates dynamic information
/// \author     Joachim Danmayr
///
void DialogProcessingController::refreshThread()
{
  while(!mStopped) {
    refreshFunction();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

///
/// \brief      Updates dynamic information
/// \author     Joachim Danmayr
///
void DialogProcessingController::refreshFunction()
{
  wxString newTextAllOver                  = "0/0";
  wxString newTextImage                    = "0/0";
  joda::pipeline::Pipeline::State actState = joda::pipeline::Pipeline::State::STOPPED;
  try {
    auto [progress, state, errorMsg] = mPipelineController->getState();
    if(state == joda::pipeline::Pipeline::State::ERROR_) {
      mLastErrorMsg = errorMsg;
    }
    if(state == joda::pipeline::Pipeline::State::RUNNING) {
      mEndedTime = std::chrono::high_resolution_clock::now();
    }
    actState = state;

    newTextAllOver = wxString::Format("%d/%d", progress.total.finished, progress.total.total);
    newTextImage   = wxString::Format("%d/%d", progress.image.finished, progress.image.total);

    mProgressImage->SetRange(progress.image.total);
    mProgressImage->SetValue(progress.image.finished);
    mProgressAllOver->SetRange(progress.total.total);
    mProgressAllOver->SetValue(progress.total.finished);

  } catch(const std::exception &ex) {
    wxCommandEvent ev;
    onStopClicked(ev);
  }
  double elapsedTimeMs = std::chrono::duration<double, std::milli>(mEndedTime - mStartedTime).count();
  auto [timeDiff, exp] = exponentForTime(elapsedTimeMs);
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << timeDiff << " " << exp;
  std::string timeDiffStr = stream.str();

  CallAfter([this, actState, newTextAllOver, newTextImage, timeDiffStr]() {
    if(!mStopped && actState == joda::pipeline::Pipeline::State::ERROR_) {
      mStopped = true;
      showErrorDialog(mLastErrorMsg);
    }
    mLabelProgressAllOver->SetLabel(newTextAllOver);
    mLabelProgressImage->SetLabel(newTextImage);
    mButtonStop->Enable(actState == joda::pipeline::Pipeline::State::RUNNING);
    mButtonClose->Enable(actState != joda::pipeline::Pipeline::State::RUNNING);
    mLabelReporting->SetLabel(timeDiffStr);
  });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogProcessingController::showErrorDialog(const std::string &what)
{
  wxMessageBox(what, "Error", wxOK | wxICON_ERROR, this);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogProcessingController::onCloseClicked(wxCommandEvent &event)
{
  try {
    mPipelineController->stop();
  } catch(const std::exception &) {
  }

  mStopped = true;
  if(mRefreshTimer->joinable()) {
    mRefreshTimer->join();
  }
  this->EndModal(wxID_CANCEL);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogProcessingController::onStopClicked(wxCommandEvent &event)
{
  try {
    mPipelineController->stop();
  } catch(const std::exception &) {
  }
}

}    // namespace joda::ui::wxwidget
