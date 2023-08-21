///
/// \file      main_window.cpp
/// \author    Joachim Danmayr
/// \date      2023-08-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include <wx/gdicmn.h>
#include <wx/mstream.h>
#include "icons/icon_16_folder.h"
#include "main_windows.hpp"

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "imageC")
{
  //
  // Build up menubar
  //
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  // Top level menu
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);

  //
  // Toolbar
  //
  wxToolBar *toolBar = CreateToolBar();
  toolBar->Show(true);

  wxMemoryInputStream stream(icon_16_folder, sizeof(icon_16_folder));
  wxImage theBitmap;
  if(!theBitmap.LoadFile(stream, wxBITMAP_TYPE_PNG))
    return;

  toolBar->AddTool(wxID_OPEN, wxT("Open"), wxBitmap(theBitmap) /*wxBitmap(wxImage(_T("icons/icon_16_folder.png")))*/);
  toolBar->Realize();

  // Build up toolbar
  CreateStatusBar();

  // Statusbar
  SetStatusText("Welcome to imageC!");

  // Bindings
  Bind(wxEVT_MENU, &MainFrame::OnHello, this, ID_Hello);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
}

void MainFrame::OnExit(wxCommandEvent &event)
{
  Close(true);
}

void MainFrame::OnAbout(wxCommandEvent &event)
{
  wxMessageBox("This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnHello(wxCommandEvent &event)
{
  wxLogMessage("Hello world from wxWidgets!");
}
