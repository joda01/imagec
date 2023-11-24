///
/// \file      panel_intersection_control.h
/// \author    Joachim Danmayr
/// \date      2023-09-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#ifndef __panel_intersection_control__
#define __panel_intersection_control__

#include "backend/settings/channel_settings.hpp"
#include "backend/settings/pipeline_settings.hpp"
#include "ui/wxwidgets/panel_ui_pipelinestep.h"
#include <nlohmann/json_fwd.hpp>
#include "wxwidget.h"

namespace joda::ui::wxwidget {

class FrameMainController;

///
/// \class      PanelIntersectionControl
/// \author     Joachim Danmayr
/// \brief      Panel intersection controller class
///
class PanelIntersectionControl : public PanelIntersection, public PanelUiPipelineStep
{
public:
  /////////////////////////////////////////////////////
  PanelIntersectionControl(FrameMainController *mainFrame, wxWindow *parent, wxWindowID id = wxID_ANY,
                           const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(-1, -1),
                           long style = wxTAB_TRAVERSAL, const wxString &name = wxEmptyString);

  void loadValues(const joda::settings::json::PipelineStepSettings &) override;
  void loadValues(const joda::settings::json::PipelineStepIntersection &);
  nlohmann::json getValues() override;

private:
  /////////////////////////////////////////////////////
  void onRemoveClicked(wxCommandEvent &event) override;
  void OnPaint(wxPaintEvent &event);

  FrameMainController *mMainFrame;
};

}    // namespace joda::ui::wxwidget
#endif    // __panel_intersection_control__
