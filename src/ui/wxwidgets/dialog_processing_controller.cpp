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
}

DialogProcessingController::~DialogProcessingController()
{
  mStopped = true;
  if(mRefreshTimer->joinable()) {
    mRefreshTimer->join();
  }
}

///
/// \brief      Updates dynamic information
/// \author     Joachim Danmayr
///
void DialogProcessingController::refreshThread()
{
  while(!mStopped) {
    refreshFunction();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
    if(state == joda::pipeline::Pipeline::State::ERROR) {
      mLastErrorMsg = errorMsg;
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

  CallAfter([this, actState, newTextAllOver, newTextImage]() {
    if(!mStopped && actState == joda::pipeline::Pipeline::State::ERROR) {
      mStopped = true;
      showErrorDialog(mLastErrorMsg);
    }
    mLabelProgressAllOver->SetLabel(newTextAllOver);
    mLabelProgressImage->SetLabel(newTextImage);
    mButtonStop->Enable(actState == joda::pipeline::Pipeline::State::RUNNING);
    mButtonClose->Enable(actState != joda::pipeline::Pipeline::State::RUNNING);
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
