#pragma once

// Start of wxWidgets "Hello World" Program
#include <wx/wx.h>
#include "app_ids.h"

///
/// \class      MainFrame
/// \author     Joachim Danmayr
/// \brief      Main window
///
class MainFrame : public wxFrame
{
public:
  MainFrame();

private:
  void OnHello(wxCommandEvent &event);
  void OnExit(wxCommandEvent &event);
  void OnAbout(wxCommandEvent &event);
};
