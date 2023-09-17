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
  mRefreshTimer = std::make_shared<std::thread>(&DialogProcessingController::refreshFunction, this);
}

DialogProcessingController::~DialogProcessingController()
{
  mStopped = true;
  mRefreshTimer->join();
}

///
/// \brief      Updates dynamic information
/// \author     Joachim Danmayr
///
void DialogProcessingController::refreshFunction()
{
  while(!mStopped) {
    try {
      auto [progress, state] = mPipelineController->getState();
      std::string image      = std::to_string(progress.image.finished) + "/" + std::to_string(progress.image.total);
      std::cout << image << std::endl;

      std::string total = std::to_string(progress.total.finished) + "/" + std::to_string(progress.total.total);
      std::cout << total << std::endl;

      // mLabelProgressImage->SetLabel(std::to_string(progress.image.finished) + "/" +
      //                               std::to_string(progress.image.total));
      // mProgressImage->SetRange(progress.image.total);
      // mProgressImage->SetValue(progress.image.finished);
      //
      // mLabelProgressAllOver->SetLabel(std::to_string(progress.total.finished) + "/" +
      //                                std::to_string(progress.total.total));
      // mProgressAllOver->SetRange(progress.total.total);
      // mProgressAllOver->SetValue(progress.total.finished);
    } catch(const std::exception &ex) {
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogProcessingController::onStartClicked(wxCommandEvent &event)
{
  mPipelineController->start(*mAnalyzeSettins);
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
  mPipelineController->stop();
}

}    // namespace joda::ui::wxwidget
