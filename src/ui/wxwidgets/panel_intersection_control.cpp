///
/// \file      panel_intersection_control.cpp
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

#include "panel_intersection_control.h"
#include "backend/settings/pipeline_settings.hpp"
#include "ui/wxwidgets/frame_main_controller.h"

namespace joda::ui::wxwidget {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelIntersectionControl::PanelIntersectionControl(FrameMainController *mainFrame, wxWindow *parent, wxWindowID id,
                                                   const wxPoint &pos, const wxSize &size, long style,
                                                   const wxString &name) :
    PanelIntersection(parent, id, pos, size, style, name),
    mMainFrame(mainFrame)
{
  Bind(wxEVT_PAINT, &PanelIntersectionControl::OnPaint, this);
}

///
/// \brief      On remove clicked
/// \author     Joachim Danmayr
/// \return
///
void PanelIntersectionControl::onRemoveClicked(wxCommandEvent &event)
{
  mMainFrame->removePipelineStepById(GetId());
}

void PanelIntersectionControl::loadValues(const joda::settings::json::PipelineStepSettings &settings)
{
  loadValues(*settings.getIntersection());
}

void PanelIntersectionControl::loadValues(const joda::settings::json::PipelineStepIntersection &intersect)
{
  mButtonIntersectionCh01->SetValue(false);
  mButtonIntersectionCh02->SetValue(false);
  mButtonIntersectionCh03->SetValue(false);
  mButtonIntersectionCh04->SetValue(false);
  mButtonIntersectionCh05->SetValue(false);
  mButtonIntersectionCh06->SetValue(false);
  mButtonIntersectionCh07->SetValue(false);
  mButtonIntersectionCh08->SetValue(false);
  mButtonIntersectionCh09->SetValue(false);
  mButtonIntersectionCh10->SetValue(false);
  mButtonIntersectionCh11->SetValue(false);
  mButtonIntersectionCh12->SetValue(false);
  mButtonIntersectionChEstimatedCell->SetValue(false);

  for(const auto idx : intersect.channel_index) {
    if(idx == 0)
      mButtonIntersectionCh01->SetValue(true);
    if(idx == 1)
      mButtonIntersectionCh02->SetValue(true);
    if(idx == 2)
      mButtonIntersectionCh03->SetValue(true);
    if(idx == 3)
      mButtonIntersectionCh04->SetValue(true);
    if(idx == 4)
      mButtonIntersectionCh05->SetValue(true);
    if(idx == 5)
      mButtonIntersectionCh06->SetValue(true);
    if(idx == 6)
      mButtonIntersectionCh07->SetValue(true);
    if(idx == 7)
      mButtonIntersectionCh08->SetValue(true);
    if(idx == 8)
      mButtonIntersectionCh09->SetValue(true);
    if(idx == 9)
      mButtonIntersectionCh10->SetValue(true);
    if(idx == 10)
      mButtonIntersectionCh11->SetValue(true);
    if(idx == 11)
      mButtonIntersectionCh12->SetValue(true);
    if(idx == static_cast<int32_t>(settings::json::PipelineStepSettings::PipelineStepIndex::CELL_APPROXIMATION))
      mButtonIntersectionChEstimatedCell->SetValue(true);
  }

  mSpinMinIntersection->SetValue(intersect.min_intersection);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
nlohmann::json PanelIntersectionControl::getValues()
{
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
    return {{"intersection",
             {{"channel_index", intersectionButtons}, {"min_intersection", mSpinMinIntersection->GetValue()}}}};
  }
  return {};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelIntersectionControl::OnPaint(wxPaintEvent &event)
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

uint64_t PanelIntersectionControl::getUniqueID()
{
  return GetId();
}
}    // namespace joda::ui::wxwidget
