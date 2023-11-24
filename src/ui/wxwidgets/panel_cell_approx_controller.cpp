#include "panel_cell_approx_controller.h"
#include "ui/wxwidgets/frame_main_controller.h"

namespace joda::ui::wxwidget {

PanelCellApproxController::PanelCellApproxController(FrameMainController *mainFrame, wxWindow *parent, wxWindowID id,
                                                     const wxPoint &pos, const wxSize &size, long style,
                                                     const wxString &name) :
    PanelCellApproximation(parent, id, pos, size, style, name),
    mMainFrame(mainFrame)
{
  Bind(wxEVT_PAINT, &PanelCellApproxController::OnPaint, this);
}

void PanelCellApproxController::onRemoveClicked(wxCommandEvent &event)
{
  mMainFrame->removePipelineStep(this);
}

void PanelCellApproxController::loadValues(const joda::settings::json::PipelineStepSettings &settings)
{
  loadValues(*settings.getCellApproximation());
}

void PanelCellApproxController::loadValues(const joda::settings::json::PipelineStepCellApproximation &pipelineStep)
{
  mChoiceCellChannel->SetSelection(pipelineStep.cell_channel_index + 1);
  mChoiceNucluesChannel->SetSelection(pipelineStep.nucleus_channel_index + 1);
  mSpinMaxCellRadius->SetValue(pipelineStep.max_cell_radius);
}

nlohmann::json PanelCellApproxController::getValues()
{
  if(mChoiceNucluesChannel->GetSelection() > 0 || mChoiceCellChannel->GetSelection() > 0) {
    return {{"cell_approximation",
             {{"nucleus_channel_index", mChoiceNucluesChannel->GetSelection() - 1},
              {"cell_channel_index", mChoiceCellChannel->GetSelection() - 1},
              {"max_cell_radius", mSpinMaxCellRadius->GetValue()}}}};
  }
  return {};
}

///
/// \brief      Cell channel choice
/// \author     Joachim Danmayr
/// \param[in]  event
///
void PanelCellApproxController::onCellChannelChoice(wxCommandEvent &event)
{
  if(mChoiceCellChannel->GetSelection() > 0) {
    panelMaxCellRadius->Show(false);
  } else {
    panelMaxCellRadius->Show(true);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelCellApproxController::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  // Set the pen color and width for the border
  wxPen borderPen(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU), 2, wxPENSTYLE_SOLID);
  dc.SetPen(borderPen);

  // Set the brush color for the panel background
  // 241, 240, 238
  wxBrush backgroundBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU), wxBRUSHSTYLE_SOLID);
  dc.SetBrush(backgroundBrush);

  // Draw the rounded rectangle
  dc.DrawRoundedRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight(), 18);
}
}    // namespace joda::ui::wxwidget
