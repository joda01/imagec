///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-370-gc831f1f7)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "noname.h"

#include "../src/res/icons8-ai-20.png.h"
#include "../src/res/icons8-all-out-20.png.h"
#include "../src/res/icons8-background-remover-20.png.h"
#include "../src/res/icons8-blur-20.png.h"
#include "../src/res/icons8-bursts-20.png.h"
#include "../src/res/icons8-centre-point-20.png.h"
#include "../src/res/icons8-contrast-20.png.h"
#include "../src/res/icons8-cpu-20.png.h"
#include "../src/res/icons8-crop-20.png.h"
#include "../src/res/icons8-electrical-threshold-20.png.h"
#include "../src/res/icons8-info-20.png.h"
#include "../src/res/icons8-lambda-20.png.h"
#include "../src/res/icons8-layers-20.png.h"
#include "../src/res/icons8-octagon-20.png.h"
#include "../src/res/icons8-opened-folder-20.png.h"
#include "../src/res/icons8-plus-math-20.png.h"
#include "../src/res/icons8-preview-20.png.h"
#include "../src/res/icons8-save-20.png.h"
#include "../src/res/icons8-settings-20.png.h"
#include "../src/res/icons8-sphere-20.png.h"
#include "../src/res/icons8-start-20.png.h"
#include "../src/res/imagec.png.h"

///////////////////////////////////////////////////////////////////////////
using namespace joda::gui;

frameMain::frameMain(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size,
                     long style) :
    wxFrame(parent, id, title, pos, size, style)
{
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  mToolBar    = this->CreateToolBar(wxTB_HORIZONTAL, wxID_ANY);
  mButtonSave = mToolBar->AddTool(wxID_ANY, _("Save"), icons8 - save - 20_png_to_wx_bitmap(), wxNullBitmap,
                                  wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

  mButtonOpen = mToolBar->AddTool(wxID_ANY, _("Open"), icons8 - opened - folder - 20_png_to_wx_bitmap(), wxNullBitmap,
                                  wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

  mToolBar->AddSeparator();

  mLabelNrOfFoundFiles =
      new wxStaticText(mToolBar, wxID_ANY, _("80"), wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTER_HORIZONTAL);
  mLabelNrOfFoundFiles->Wrap(-1);
  mLabelNrOfFoundFiles->SetMinSize(wxSize(50, -1));

  mToolBar->AddControl(mLabelNrOfFoundFiles);
  mTextWorkingDirectory =
      new wxTextCtrl(mToolBar, wxID_ANY, _("Direcotry of files to analyze..."), wxDefaultPosition, wxSize(500, -1), 0);
  mTextWorkingDirectory->SetMinSize(wxSize(500, -1));

  mToolBar->AddControl(mTextWorkingDirectory);
  mButtonSelectWorkingDirectory =
      mToolBar->AddTool(wxID_ANY, _("Stop"), icons8 - opened - folder - 20_png_to_wx_bitmap(), wxNullBitmap,
                        wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

  mButtonRun = mToolBar->AddTool(wxID_ANY, _("Start"), icons8 - start - 20_png_to_wx_bitmap(), wxNullBitmap,
                                 wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

  mToolBar->AddSeparator();

  mButtonSettings = mToolBar->AddTool(wxID_ANY, _("Settings"), icons8 - settings - 20_png_to_wx_bitmap(), wxNullBitmap,
                                      wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

  mButtonAbout = mToolBar->AddTool(wxID_ANY, _("Info"), icons8 - info - 20_png_to_wx_bitmap(), wxNullBitmap,
                                   wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

  mToolBar->Realize();

  wxBoxSizer *mSizerMain;
  mSizerMain = new wxBoxSizer(wxVERTICAL);

  mNotebookMain = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
  mPanelChannel = new wxPanel(mNotebookMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer *mSizerChannels;
  mSizerChannels = new wxBoxSizer(wxHORIZONTAL);

  mPannelChannel =
      new wxScrolledWindow(mPanelChannel, wxID_ANY, wxDefaultPosition, wxSize(250, -1), wxBORDER_NONE | wxVSCROLL);
  mPannelChannel->SetScrollRate(5, 5);
  mPannelChannel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
  mPannelChannel->SetMinSize(wxSize(250, -1));
  mPannelChannel->SetMaxSize(wxSize(250, -1));

  wxGridSizer *mSizerGridChannel;
  mSizerGridChannel = new wxGridSizer(0, 1, 0, 0);

  wxBoxSizer *mSizerChannel;
  mSizerChannel = new wxBoxSizer(wxVERTICAL);

  mLabelChannelTitle = new wxStaticText(mPannelChannel, wxID_ANY, _("Channel"), wxDefaultPosition, wxDefaultSize,
                                        wxALIGN_CENTER_HORIZONTAL);
  mLabelChannelTitle->Wrap(-1);
  mLabelChannelTitle->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                                     wxFONTWEIGHT_BOLD, false, wxEmptyString));

  mSizerChannel->Add(mLabelChannelTitle, 0, wxALIGN_CENTER | wxALL | wxEXPAND, 5);

  mTextChannelName = new wxTextCtrl(mPannelChannel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
  mSizerChannel->Add(mTextChannelName, 0, wxALL | wxEXPAND, 5);

  panelChannelType = new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  panelChannelType->SetMaxSize(wxSize(-1, 35));

  wxBoxSizer *sizerChannelType;
  sizerChannelType = new wxBoxSizer(wxVERTICAL);

  wxString mChoiceChannelTypeChoices[] = {_("Spot"), _("Nucleus"), _("Cell"), _("Background")};
  int mChoiceChannelTypeNChoices       = sizeof(mChoiceChannelTypeChoices) / sizeof(wxString);
  mChoiceChannelType                   = new wxChoice(panelChannelType, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                      mChoiceChannelTypeNChoices, mChoiceChannelTypeChoices, 0);
  mChoiceChannelType->SetSelection(0);
  sizerChannelType->Add(mChoiceChannelType, 0, wxEXPAND, 5);

  panelChannelType->SetSizer(sizerChannelType);
  panelChannelType->Layout();
  sizerChannelType->Fit(panelChannelType);
  mSizerChannel->Add(panelChannelType, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  panelChannelIndex = new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  panelChannelIndex->SetMaxSize(wxSize(-1, 35));

  wxBoxSizer *sizerChannelIndex;
  sizerChannelIndex = new wxBoxSizer(wxVERTICAL);

  wxString mChoiceChannelIndexChoices[] = {_("Channel 1"), _("Channel 2"),  _("Channel 3"),  _("Channel 4"),
                                           _("Channel 5"), _("Channel 6"),  _("Channel 7"),  _("Channel 8"),
                                           _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12")};
  int mChoiceChannelIndexNChoices       = sizeof(mChoiceChannelIndexChoices) / sizeof(wxString);
  mChoiceChannelIndex                   = new wxChoice(panelChannelIndex, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                       mChoiceChannelIndexNChoices, mChoiceChannelIndexChoices, 0);
  mChoiceChannelIndex->SetSelection(0);
  sizerChannelIndex->Add(mChoiceChannelIndex, 0, wxEXPAND, 5);

  panelChannelIndex->SetSizer(sizerChannelIndex);
  panelChannelIndex->Layout();
  sizerChannelIndex->Fit(panelChannelIndex);
  mSizerChannel->Add(panelChannelIndex, 1, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mLabelPreprocessing =
      new wxStaticText(mPannelChannel, wxID_ANY, _("Preprocessing"), wxDefaultPosition, wxDefaultSize, 0);
  mLabelPreprocessing->Wrap(-1);
  mLabelPreprocessing->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD, false, wxEmptyString));

  mSizerChannel->Add(mLabelPreprocessing, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mLinePreprocessing = new wxStaticLine(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  mLinePreprocessing->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

  mSizerChannel->Add(mLinePreprocessing, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT, 5);

  panelZStack =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelZStack->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerZStack;
  sizerZStack = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerZStack2;
  sizerZStack2 = new wxBoxSizer(wxHORIZONTAL);

  iconZStack = new wxStaticBitmap(panelZStack, wxID_ANY, icons8 - layers - 20_png_to_wx_bitmap(), wxDefaultPosition,
                                  wxDefaultSize, 0);
  iconZStack->SetMaxSize(wxSize(20, -1));

  sizerZStack2->Add(iconZStack, 0, wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  wxString mChoiceZStackChoices[] = {_("Off"), _("Max. intensity projection"), _("3D projection")};
  int mChoiceZStackNChoices       = sizeof(mChoiceZStackChoices) / sizeof(wxString);
  mChoiceZStack = new wxChoice(panelZStack, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceZStackNChoices,
                               mChoiceZStackChoices, 0);
  mChoiceZStack->SetSelection(0);
  sizerZStack2->Add(mChoiceZStack, 1, wxEXPAND, 5);

  sizerZStack->Add(sizerZStack2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelZStack =
      new wxStaticText(panelZStack, wxID_ANY, _("Z-stack projection"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  mLabelZStack->Wrap(-1);
  mLabelZStack->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerZStack->Add(mLabelZStack, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelZStack->SetSizer(sizerZStack);
  panelZStack->Layout();
  sizerZStack->Fit(panelZStack);
  mSizerChannel->Add(panelZStack, 1, wxEXPAND | wxTOP, 5);

  panelMarginCrop =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelMarginCrop->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerMarginCrop;
  sizerMarginCrop = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerMarginCrop2;
  sizerMarginCrop2 = new wxBoxSizer(wxHORIZONTAL);

  iconMarginCrop = new wxStaticBitmap(panelMarginCrop, wxID_ANY, icons8 - crop - 20_png_to_wx_bitmap(),
                                      wxDefaultPosition, wxDefaultSize, 0);
  iconMarginCrop->SetMaxSize(wxSize(20, -1));

  sizerMarginCrop2->Add(iconMarginCrop, 0, wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT,
                        5);

  mSpinMarginCrop = new wxSpinCtrlDouble(panelMarginCrop, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                         wxSP_ARROW_KEYS, 0, 65535, 0.000000, 1);
  mSpinMarginCrop->SetDigits(0);
  sizerMarginCrop2->Add(mSpinMarginCrop, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

  sizerMarginCrop->Add(sizerMarginCrop2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelMarginCrop = new wxStaticText(panelMarginCrop, wxID_ANY, _("Margin crop [px]"), wxDefaultPosition,
                                      wxDefaultSize, wxALIGN_LEFT);
  mLabelMarginCrop->Wrap(-1);
  mLabelMarginCrop->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerMarginCrop->Add(mLabelMarginCrop, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelMarginCrop->SetSizer(sizerMarginCrop);
  panelMarginCrop->Layout();
  sizerMarginCrop->Fit(panelMarginCrop);
  mSizerChannel->Add(panelMarginCrop, 1, wxEXPAND | wxTOP, 5);

  panelMedianBGSubtract =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelMedianBGSubtract->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerMedianBGSubtract;
  sizerMedianBGSubtract = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerMedianBGSubtract2;
  sizerMedianBGSubtract2 = new wxBoxSizer(wxHORIZONTAL);

  iconMedianBGSubtract =
      new wxStaticBitmap(panelMedianBGSubtract, wxID_ANY, icons8 - electrical - threshold - 20_png_to_wx_bitmap(),
                         wxDefaultPosition, wxDefaultSize, 0);
  iconMedianBGSubtract->SetMaxSize(wxSize(20, -1));

  sizerMedianBGSubtract2->Add(iconMedianBGSubtract, 0,
                              wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  wxString mChoiceMedianBGSubtractChoices[] = {_("Off"), _("On")};
  int mChoiceMedianBGSubtractNChoices       = sizeof(mChoiceMedianBGSubtractChoices) / sizeof(wxString);
  mChoiceMedianBGSubtract = new wxChoice(panelMedianBGSubtract, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         mChoiceMedianBGSubtractNChoices, mChoiceMedianBGSubtractChoices, 0);
  mChoiceMedianBGSubtract->SetSelection(1);
  sizerMedianBGSubtract2->Add(mChoiceMedianBGSubtract, 1, wxEXPAND, 5);

  sizerMedianBGSubtract->Add(sizerMedianBGSubtract2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelMedianBGSubtract = new wxStaticText(panelMedianBGSubtract, wxID_ANY, _("Median based BG substraction"),
                                            wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  mLabelMedianBGSubtract->Wrap(-1);
  mLabelMedianBGSubtract->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerMedianBGSubtract->Add(mLabelMedianBGSubtract, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelMedianBGSubtract->SetSizer(sizerMedianBGSubtract);
  panelMedianBGSubtract->Layout();
  sizerMedianBGSubtract->Fit(panelMedianBGSubtract);
  mSizerChannel->Add(panelMedianBGSubtract, 1, wxEXPAND | wxTOP, 5);

  panelRollingBall =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelRollingBall->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerRollingBall;
  sizerRollingBall = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerRollingBall2;
  sizerRollingBall2 = new wxBoxSizer(wxHORIZONTAL);

  iconRollingBall = new wxStaticBitmap(panelRollingBall, wxID_ANY, icons8 - sphere - 20_png_to_wx_bitmap(),
                                       wxDefaultPosition, wxDefaultSize, 0);
  iconRollingBall->SetMaxSize(wxSize(20, -1));

  sizerRollingBall2->Add(iconRollingBall, 0,
                         wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  mSpinRollingBall = new wxSpinCtrlDouble(panelRollingBall, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                          wxSP_ARROW_KEYS, 0, 256, 0, 1);
  mSpinRollingBall->SetDigits(0);
  sizerRollingBall2->Add(mSpinRollingBall, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

  sizerRollingBall->Add(sizerRollingBall2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelRollingBall = new wxStaticText(panelRollingBall, wxID_ANY, _("Rolling Ball BG substraction"), wxDefaultPosition,
                                       wxDefaultSize, wxALIGN_LEFT);
  mLabelRollingBall->Wrap(-1);
  mLabelRollingBall->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerRollingBall->Add(mLabelRollingBall, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelRollingBall->SetSizer(sizerRollingBall);
  panelRollingBall->Layout();
  sizerRollingBall->Fit(panelRollingBall);
  mSizerChannel->Add(panelRollingBall, 1, wxEXPAND | wxTOP, 5);

  panelBGSubtraction =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelBGSubtraction->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerBGSubtraction;
  sizerBGSubtraction = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerBGSubtraction2;
  sizerBGSubtraction2 = new wxBoxSizer(wxHORIZONTAL);

  iconBGSubtraction =
      new wxStaticBitmap(panelBGSubtraction, wxID_ANY, icons8 - background - remover - 20_png_to_wx_bitmap(),
                         wxDefaultPosition, wxDefaultSize, 0);
  iconBGSubtraction->SetMaxSize(wxSize(20, -1));

  sizerBGSubtraction2->Add(iconBGSubtraction, 0,
                           wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  wxString mSpinBGSubtractionChoices[] = {
      _("Off"),       _("Channel 1"), _("Channel 2"), _("Channel 3"),  _("Channel 4"),  _("Channel 5"), _("Channel 6"),
      _("Channel 7"), _("Channel 8"), _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12")};
  int mSpinBGSubtractionNChoices = sizeof(mSpinBGSubtractionChoices) / sizeof(wxString);
  mSpinBGSubtraction             = new wxChoice(panelBGSubtraction, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                mSpinBGSubtractionNChoices, mSpinBGSubtractionChoices, 0);
  mSpinBGSubtraction->SetSelection(0);
  sizerBGSubtraction2->Add(mSpinBGSubtraction, 1, wxEXPAND, 5);

  sizerBGSubtraction->Add(sizerBGSubtraction2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelBGSubtraction = new wxStaticText(panelBGSubtraction, wxID_ANY, _("Channel based BG substraction"),
                                         wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  mLabelBGSubtraction->Wrap(-1);
  mLabelBGSubtraction->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerBGSubtraction->Add(mLabelBGSubtraction, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelBGSubtraction->SetSizer(sizerBGSubtraction);
  panelBGSubtraction->Layout();
  sizerBGSubtraction->Fit(panelBGSubtraction);
  mSizerChannel->Add(panelBGSubtraction, 1, wxEXPAND | wxTOP, 5);

  panelBluer =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelBluer->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerBluer;
  sizerBluer = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerBluer2;
  sizerBluer2 = new wxBoxSizer(wxHORIZONTAL);

  iconBluer = new wxStaticBitmap(panelBluer, wxID_ANY, icons8 - blur - 20_png_to_wx_bitmap(), wxDefaultPosition,
                                 wxDefaultSize, 0);
  iconBluer->SetMaxSize(wxSize(20, -1));

  sizerBluer2->Add(iconBluer, 0, wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  mSpinBluer = new wxSpinCtrlDouble(panelBluer, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    wxSP_ARROW_KEYS, 0, 128, 0, 1);
  mSpinBluer->SetDigits(0);
  sizerBluer2->Add(mSpinBluer, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

  sizerBluer->Add(sizerBluer2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelBluer = new wxStaticText(panelBluer, wxID_ANY, _("Bluer nois reduction [0-65535]"), wxDefaultPosition,
                                 wxDefaultSize, wxALIGN_LEFT);
  mLabelBluer->Wrap(-1);
  mLabelBluer->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerBluer->Add(mLabelBluer, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelBluer->SetSizer(sizerBluer);
  panelBluer->Layout();
  sizerBluer->Fit(panelBluer);
  mSizerChannel->Add(panelBluer, 1, wxEXPAND | wxTOP, 5);

  mLabelDescription = new wxStaticText(mPannelChannel, wxID_ANY, _("Detection"), wxDefaultPosition, wxDefaultSize, 0);
  mLabelDescription->Wrap(-1);
  mLabelDescription->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD, false, wxEmptyString));

  mSizerChannel->Add(mLabelDescription, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mLineDescription = new wxStaticLine(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  mLineDescription->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

  mSizerChannel->Add(mLineDescription, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT, 5);

  panelUseAI =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelUseAI->SetMaxSize(wxSize(-1, 35));

  wxBoxSizer *sizerUseAI;
  sizerUseAI = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerUseAI2;
  sizerUseAI2 = new wxBoxSizer(wxHORIZONTAL);

  iconUseAI = new wxStaticBitmap(panelUseAI, wxID_ANY, icons8 - ai - 20_png_to_wx_bitmap(), wxDefaultPosition,
                                 wxDefaultSize, 0);
  iconUseAI->SetMaxSize(wxSize(20, -1));

  sizerUseAI2->Add(iconUseAI, 0, wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  mCheckUseAI = new wxCheckBox(panelUseAI, wxID_ANY, _("Use AI"), wxDefaultPosition, wxDefaultSize, 0);
  sizerUseAI2->Add(mCheckUseAI, 0, wxEXPAND, 5);

  sizerUseAI->Add(sizerUseAI2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  panelUseAI->SetSizer(sizerUseAI);
  panelUseAI->Layout();
  sizerUseAI->Fit(panelUseAI);
  mSizerChannel->Add(panelUseAI, 1, wxEXPAND | wxTOP, 5);

  panelThresholdMethod =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelThresholdMethod->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *ThresholdMethod;
  ThresholdMethod = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *izerThresholdMethod2;
  izerThresholdMethod2 = new wxBoxSizer(wxHORIZONTAL);

  iconThresholdMethod = new wxStaticBitmap(panelThresholdMethod, wxID_ANY, icons8 - lambda - 20_png_to_wx_bitmap(),
                                           wxDefaultPosition, wxDefaultSize, 0);
  iconThresholdMethod->SetMaxSize(wxSize(20, -1));

  izerThresholdMethod2->Add(iconThresholdMethod, 0,
                            wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  wxString mChoiceThresholdMethodChoices[] = {_("Manual"), _("Li"), _("Triangle")};
  int mChoiceThresholdMethodNChoices       = sizeof(mChoiceThresholdMethodChoices) / sizeof(wxString);
  mChoiceThresholdMethod = new wxChoice(panelThresholdMethod, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        mChoiceThresholdMethodNChoices, mChoiceThresholdMethodChoices, 0);
  mChoiceThresholdMethod->SetSelection(2);
  izerThresholdMethod2->Add(mChoiceThresholdMethod, 1, wxEXPAND, 5);

  ThresholdMethod->Add(izerThresholdMethod2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelThresholdMethod = new wxStaticText(panelThresholdMethod, wxID_ANY, _("Threshold method"), wxDefaultPosition,
                                           wxDefaultSize, wxALIGN_LEFT);
  mLabelThresholdMethod->Wrap(-1);
  mLabelThresholdMethod->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  ThresholdMethod->Add(mLabelThresholdMethod, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelThresholdMethod->SetSizer(ThresholdMethod);
  panelThresholdMethod->Layout();
  ThresholdMethod->Fit(panelThresholdMethod);
  mSizerChannel->Add(panelThresholdMethod, 1, wxEXPAND | wxTOP, 5);

  panelMinThreshold =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelMinThreshold->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerMinThreshold;
  sizerMinThreshold = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerMinThreshold2;
  sizerMinThreshold2 = new wxBoxSizer(wxHORIZONTAL);

  iconMinThreshold = new wxStaticBitmap(panelMinThreshold, wxID_ANY, icons8 - contrast - 20_png_to_wx_bitmap(),
                                        wxDefaultPosition, wxDefaultSize, 0);
  iconMinThreshold->SetMaxSize(wxSize(20, -1));

  sizerMinThreshold2->Add(iconMinThreshold, 0,
                          wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  mSpinMinThreshold = new wxSpinCtrlDouble(panelMinThreshold, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                           wxSP_ARROW_KEYS, 0, 65535, 0, 1);
  mSpinMinThreshold->SetDigits(0);
  sizerMinThreshold2->Add(mSpinMinThreshold, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

  sizerMinThreshold->Add(sizerMinThreshold2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelMinThreshold = new wxStaticText(panelMinThreshold, wxID_ANY, _("Min. Threshold [0-65535)"), wxDefaultPosition,
                                        wxDefaultSize, wxALIGN_LEFT);
  mLabelMinThreshold->Wrap(-1);
  mLabelMinThreshold->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerMinThreshold->Add(mLabelMinThreshold, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelMinThreshold->SetSizer(sizerMinThreshold);
  panelMinThreshold->Layout();
  sizerMinThreshold->Fit(panelMinThreshold);
  mSizerChannel->Add(panelMinThreshold, 1, wxEXPAND | wxTOP, 5);

  mLabelFilter = new wxStaticText(mPannelChannel, wxID_ANY, _("Filtering"), wxDefaultPosition, wxDefaultSize, 0);
  mLabelFilter->Wrap(-1);
  mLabelFilter->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD, false, wxEmptyString));

  mSizerChannel->Add(mLabelFilter, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mLineFilter = new wxStaticLine(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  mLineFilter->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
  mLineFilter->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

  mSizerChannel->Add(mLineFilter, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT, 5);

  panelMinCircularity =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelMinCircularity->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerMinCircularity;
  sizerMinCircularity = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerMinCircularity2;
  sizerMinCircularity2 = new wxBoxSizer(wxHORIZONTAL);

  iconMinCircularity = new wxStaticBitmap(panelMinCircularity, wxID_ANY, icons8 - octagon - 20_png_to_wx_bitmap(),
                                          wxDefaultPosition, wxDefaultSize, 0);
  iconMinCircularity->SetMaxSize(wxSize(20, -1));

  sizerMinCircularity2->Add(iconMinCircularity, 0,
                            wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  mSpinMinCircularity = new wxSpinCtrlDouble(panelMinCircularity, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                             wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0.000000, 1);
  mSpinMinCircularity->SetDigits(0);
  sizerMinCircularity2->Add(mSpinMinCircularity, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

  sizerMinCircularity->Add(sizerMinCircularity2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelMinCircularity = new wxStaticText(panelMinCircularity, wxID_ANY, _("Min. circulartiy [%]"), wxDefaultPosition,
                                          wxDefaultSize, wxALIGN_LEFT);
  mLabelMinCircularity->Wrap(-1);
  mLabelMinCircularity->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerMinCircularity->Add(mLabelMinCircularity, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelMinCircularity->SetSizer(sizerMinCircularity);
  panelMinCircularity->Layout();
  sizerMinCircularity->Fit(panelMinCircularity);
  mSizerChannel->Add(panelMinCircularity, 1, wxEXPAND | wxTOP, 5);

  panelParticleSize =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelParticleSize->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerParticleSize;
  sizerParticleSize = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerH1121;
  sizerH1121 = new wxBoxSizer(wxHORIZONTAL);

  iDescription1121 = new wxStaticBitmap(panelParticleSize, wxID_ANY, icons8 - all - out - 20_png_to_wx_bitmap(),
                                        wxDefaultPosition, wxDefaultSize, 0);
  iDescription1121->SetMaxSize(wxSize(20, -1));

  sizerH1121->Add(iDescription1121, 0, wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  m_textCtrl3 = new wxTextCtrl(panelParticleSize, wxID_ANY, _("5-9999"), wxDefaultPosition, wxDefaultSize, 0);
  sizerH1121->Add(m_textCtrl3, 1, wxEXPAND, 5);

  sizerParticleSize->Add(sizerH1121, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  lDescription1121 = new wxStaticText(panelParticleSize, wxID_ANY, _("Particle size range [px²]"), wxDefaultPosition,
                                      wxDefaultSize, wxALIGN_LEFT);
  lDescription1121->Wrap(-1);
  lDescription1121->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerParticleSize->Add(lDescription1121, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelParticleSize->SetSizer(sizerParticleSize);
  panelParticleSize->Layout();
  sizerParticleSize->Fit(panelParticleSize);
  mSizerChannel->Add(panelParticleSize, 1, wxEXPAND | wxTOP, 5);

  panelParticleSize111 =
      new wxPanel(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelParticleSize111->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerParticleSize;
  sizerParticleSize = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerParticleSize2;
  sizerParticleSize2 = new wxBoxSizer(wxHORIZONTAL);

  iconParticleSize = new wxStaticBitmap(panelParticleSize111, wxID_ANY, icons8 - centre - point - 20_png_to_wx_bitmap(),
                                        wxDefaultPosition, wxDefaultSize, 0);
  iconParticleSize->SetMaxSize(wxSize(20, -1));

  sizerParticleSize2->Add(iconParticleSize, 0,
                          wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  mSpinParticleSize = new wxSpinCtrlDouble(panelParticleSize111, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                           wxDefaultSize, wxSP_ARROW_KEYS, 0, 256, 0, 1);
  mSpinParticleSize->SetDigits(0);
  sizerParticleSize2->Add(mSpinParticleSize, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

  sizerParticleSize->Add(sizerParticleSize2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelParticleSize = new wxStaticText(panelParticleSize111, wxID_ANY, _("Snap area diamater [px²]"),
                                        wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  mLabelParticleSize->Wrap(-1);
  mLabelParticleSize->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerParticleSize->Add(mLabelParticleSize, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelParticleSize111->SetSizer(sizerParticleSize);
  panelParticleSize111->Layout();
  sizerParticleSize->Fit(panelParticleSize111);
  mSizerChannel->Add(panelParticleSize111, 1, wxEXPAND | wxTOP, 5);

  mLinePreview = new wxStaticLine(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  mLinePreview->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
  mLinePreview->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

  mSizerChannel->Add(mLinePreview, 0, wxEXPAND | wxALL, 5);

  mButtonPreview = new wxButton(mPannelChannel, wxID_ANY, _("Preview"), wxDefaultPosition, wxDefaultSize, 0);

  mButtonPreview->SetBitmap(icons8 - preview - 20_png_to_wx_bitmap());
  mSizerChannel->Add(mButtonPreview, 0, wxALIGN_CENTER | wxALL, 5);

  mLineRemove = new wxStaticLine(mPannelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  mLineRemove->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
  mLineRemove->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

  mSizerChannel->Add(mLineRemove, 0, wxEXPAND | wxALL, 5);

  mButtonRemoveChannel = new wxButton(mPannelChannel, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0);
  mButtonRemoveChannel->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
  mButtonRemoveChannel->SetBackgroundColour(wxColour(200, 110, 110));

  mSizerChannel->Add(mButtonRemoveChannel, 0, wxALIGN_CENTER | wxALL, 5);

  mSizerChannel->Add(0, 0, 1, wxEXPAND | wxSHAPED, 5);

  mSizerGridChannel->Add(mSizerChannel, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

  mPannelChannel->SetSizer(mSizerGridChannel);
  mPannelChannel->Layout();
  mSizerChannels->Add(mPannelChannel, 1, wxEXPAND | wxALL, 5);

  mButtonAddChannel = new wxButton(mPanelChannel, wxID_ANY, _("Add Channel"), wxDefaultPosition, wxDefaultSize, 0);

  mButtonAddChannel->SetBitmap(icons8 - plus - math - 20_png_to_wx_bitmap());
  mSizerChannels->Add(mButtonAddChannel, 0, wxALIGN_CENTER | wxALL, 5);

  mPanelChannel->SetSizer(mSizerChannels);
  mPanelChannel->Layout();
  mSizerChannels->Fit(mPanelChannel);
  mNotebookMain->AddPage(mPanelChannel, _("Channels"), true);
  mPanelPipelineSteps = new wxPanel(mNotebookMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer *mSizerPipelineSteps;
  mSizerPipelineSteps = new wxBoxSizer(wxHORIZONTAL);

  panelPipelineStepCellApproximation = new wxScrolledWindow(mPanelPipelineSteps, wxID_ANY, wxDefaultPosition,
                                                            wxSize(250, -1), wxBORDER_NONE | wxVSCROLL);
  panelPipelineStepCellApproximation->SetScrollRate(5, 5);
  panelPipelineStepCellApproximation->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
  panelPipelineStepCellApproximation->SetMinSize(wxSize(250, -1));
  panelPipelineStepCellApproximation->SetMaxSize(wxSize(250, -1));

  wxGridSizer *mGridCellEstimation;
  mGridCellEstimation = new wxGridSizer(0, 1, 0, 0);

  wxBoxSizer *mSizerCellEstimation;
  mSizerCellEstimation = new wxBoxSizer(wxVERTICAL);

  mLabelCellEstimation = new wxStaticText(panelPipelineStepCellApproximation, wxID_ANY, _("Cell estimation"),
                                          wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  mLabelCellEstimation->Wrap(-1);
  mLabelCellEstimation->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                                       wxFONTWEIGHT_BOLD, false, wxEmptyString));

  mSizerCellEstimation->Add(mLabelCellEstimation, 0, wxALIGN_CENTER | wxALL | wxEXPAND, 5);

  panelNucleusChannel = new wxPanel(panelPipelineStepCellApproximation, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                    wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelNucleusChannel->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerNucleusChannel;
  sizerNucleusChannel = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerNucleusChannel2;
  sizerNucleusChannel2 = new wxBoxSizer(wxHORIZONTAL);

  iconNucluesChannel = new wxStaticBitmap(panelNucleusChannel, wxID_ANY, icons8 - bursts - 20_png_to_wx_bitmap(),
                                          wxDefaultPosition, wxDefaultSize, 0);
  iconNucluesChannel->SetMaxSize(wxSize(20, -1));

  sizerNucleusChannel2->Add(iconNucluesChannel, 0,
                            wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  wxString mChoiceNucluesChannelChoices[] = {
      _("Off"),       _("Channel 1"), _("Channel 2"), _("Channel 3"),  _("Channel 4"),  _("Channel 5"), _("Channel 6"),
      _("Channel 7"), _("Channel 8"), _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12")};
  int mChoiceNucluesChannelNChoices = sizeof(mChoiceNucluesChannelChoices) / sizeof(wxString);
  mChoiceNucluesChannel             = new wxChoice(panelNucleusChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                   mChoiceNucluesChannelNChoices, mChoiceNucluesChannelChoices, 0);
  mChoiceNucluesChannel->SetSelection(0);
  sizerNucleusChannel2->Add(mChoiceNucluesChannel, 1, wxEXPAND, 5);

  sizerNucleusChannel->Add(sizerNucleusChannel2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelNucleusChannel = new wxStaticText(panelNucleusChannel, wxID_ANY, _("Nucleus channel"), wxDefaultPosition,
                                          wxDefaultSize, wxALIGN_LEFT);
  mLabelNucleusChannel->Wrap(-1);
  mLabelNucleusChannel->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerNucleusChannel->Add(mLabelNucleusChannel, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelNucleusChannel->SetSizer(sizerNucleusChannel);
  panelNucleusChannel->Layout();
  sizerNucleusChannel->Fit(panelNucleusChannel);
  mSizerCellEstimation->Add(panelNucleusChannel, 1, wxEXPAND | wxTOP, 5);

  panelMaxCellRadius = new wxPanel(panelPipelineStepCellApproximation, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelMaxCellRadius->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerMaxCellRadius;
  sizerMaxCellRadius = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerMaxCellRadius2;
  sizerMaxCellRadius2 = new wxBoxSizer(wxHORIZONTAL);

  iconMaxCellRadius = new wxStaticBitmap(panelMaxCellRadius, wxID_ANY, icons8 - all - out - 20_png_to_wx_bitmap(),
                                         wxDefaultPosition, wxDefaultSize, 0);
  iconMaxCellRadius->SetMaxSize(wxSize(20, -1));

  sizerMaxCellRadius2->Add(iconMaxCellRadius, 0,
                           wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  mSpinMaxCellRadius = new wxSpinCtrlDouble(panelMaxCellRadius, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                            wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0.000000, 1);
  mSpinMaxCellRadius->SetDigits(0);
  sizerMaxCellRadius2->Add(mSpinMaxCellRadius, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

  sizerMaxCellRadius->Add(sizerMaxCellRadius2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelMaxCellRadius = new wxStaticText(panelMaxCellRadius, wxID_ANY, _("Maximum cell radius [px²]"),
                                         wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  mLabelMaxCellRadius->Wrap(-1);
  mLabelMaxCellRadius->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerMaxCellRadius->Add(mLabelMaxCellRadius, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelMaxCellRadius->SetSizer(sizerMaxCellRadius);
  panelMaxCellRadius->Layout();
  sizerMaxCellRadius->Fit(panelMaxCellRadius);
  mSizerCellEstimation->Add(panelMaxCellRadius, 1, wxEXPAND | wxTOP, 5);

  mGridCellEstimation->Add(mSizerCellEstimation, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

  panelPipelineStepCellApproximation->SetSizer(mGridCellEstimation);
  panelPipelineStepCellApproximation->Layout();
  mSizerPipelineSteps->Add(panelPipelineStepCellApproximation, 1, wxALL | wxEXPAND, 5);

  mPanelSpotRemoval = new wxScrolledWindow(mPanelPipelineSteps, wxID_ANY, wxDefaultPosition, wxSize(250, -1),
                                           wxBORDER_NONE | wxVSCROLL);
  mPanelSpotRemoval->SetScrollRate(5, 5);
  mPanelSpotRemoval->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
  mPanelSpotRemoval->SetMinSize(wxSize(250, -1));
  mPanelSpotRemoval->SetMaxSize(wxSize(250, -1));

  wxGridSizer *mGridSpotRemoval;
  mGridSpotRemoval = new wxGridSizer(0, 1, 0, 0);

  wxBoxSizer *mSizerSpotRemoval;
  mSizerSpotRemoval = new wxBoxSizer(wxVERTICAL);

  mLabelSpotRemoval = new wxStaticText(mPanelSpotRemoval, wxID_ANY, _("Spot removal"), wxDefaultPosition, wxDefaultSize,
                                       wxALIGN_CENTER_HORIZONTAL);
  mLabelSpotRemoval->Wrap(-1);
  mLabelSpotRemoval->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                                    wxFONTWEIGHT_BOLD, false, wxEmptyString));

  mSizerSpotRemoval->Add(mLabelSpotRemoval, 0, wxALIGN_CENTER | wxALL | wxEXPAND, 5);

  panelTetraspeckChannel =
      new wxPanel(mPanelSpotRemoval, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxTAB_TRAVERSAL);
  panelTetraspeckChannel->SetMaxSize(wxSize(-1, 55));

  wxBoxSizer *sizerTetraspeckChannel;
  sizerTetraspeckChannel = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerTetraspeckChannel2;
  sizerTetraspeckChannel2 = new wxBoxSizer(wxHORIZONTAL);

  iconTetraspeckChannel = new wxStaticBitmap(panelTetraspeckChannel, wxID_ANY, icons8 - bursts - 20_png_to_wx_bitmap(),
                                             wxDefaultPosition, wxDefaultSize, 0);
  iconTetraspeckChannel->SetMaxSize(wxSize(20, -1));

  sizerTetraspeckChannel2->Add(iconTetraspeckChannel, 0,
                               wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT, 5);

  wxString mChoiceTetraspeckChannelChoices[] = {
      _("Off"),       _("Channel 1"), _("Channel 2"), _("Channel 3"),  _("Channel 4"),  _("Channel 5"), _("Channel 6"),
      _("Channel 7"), _("Channel 8"), _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12")};
  int mChoiceTetraspeckChannelNChoices = sizeof(mChoiceTetraspeckChannelChoices) / sizeof(wxString);
  mChoiceTetraspeckChannel = new wxChoice(panelTetraspeckChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          mChoiceTetraspeckChannelNChoices, mChoiceTetraspeckChannelChoices, 0);
  mChoiceTetraspeckChannel->SetSelection(0);
  sizerTetraspeckChannel2->Add(mChoiceTetraspeckChannel, 1, wxEXPAND, 5);

  sizerTetraspeckChannel->Add(sizerTetraspeckChannel2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelTetraspeckChannel = new wxStaticText(panelTetraspeckChannel, wxID_ANY, _("Tetraspeck Spot channel"),
                                             wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  mLabelTetraspeckChannel->Wrap(-1);
  mLabelTetraspeckChannel->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerTetraspeckChannel->Add(mLabelTetraspeckChannel, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelTetraspeckChannel->SetSizer(sizerTetraspeckChannel);
  panelTetraspeckChannel->Layout();
  sizerTetraspeckChannel->Fit(panelTetraspeckChannel);
  mSizerSpotRemoval->Add(panelTetraspeckChannel, 1, wxEXPAND | wxTOP, 5);

  mGridSpotRemoval->Add(mSizerSpotRemoval, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

  mPanelSpotRemoval->SetSizer(mGridSpotRemoval);
  mPanelSpotRemoval->Layout();
  mSizerPipelineSteps->Add(mPanelSpotRemoval, 1, wxEXPAND | wxALL, 5);

  mPanelIntersection = new wxScrolledWindow(mPanelPipelineSteps, wxID_ANY, wxDefaultPosition, wxSize(250, -1),
                                            wxBORDER_NONE | wxVSCROLL);
  mPanelIntersection->SetScrollRate(5, 5);
  mPanelIntersection->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
  mPanelIntersection->SetMinSize(wxSize(250, -1));
  mPanelIntersection->SetMaxSize(wxSize(250, -1));

  wxGridSizer *mGridIntersection;
  mGridIntersection = new wxGridSizer(0, 1, 0, 0);

  wxBoxSizer *mSizerIntersection;
  mSizerIntersection = new wxBoxSizer(wxVERTICAL);

  mLabelIntersection = new wxStaticText(mPanelIntersection, wxID_ANY, _("Intersection"), wxDefaultPosition,
                                        wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  mLabelIntersection->Wrap(-1);
  mLabelIntersection->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                                     wxFONTWEIGHT_BOLD, false, wxEmptyString));

  mSizerIntersection->Add(mLabelIntersection, 0, wxALIGN_CENTER | wxALL | wxEXPAND, 5);

  mPanelIntersectionButtons =
      new wxPanel(mPanelIntersection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  wxGridSizer *mGridIntersection;
  mGridIntersection = new wxGridSizer(0, 2, 0, 0);

  mButtonIntersectionCh01 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 01"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh01, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh02 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 02"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh02, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh03 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 03"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh03, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh04 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 04"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh04, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh05 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 05"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh05, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh06 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 06"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh06, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh07 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 07"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh07, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh08 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 08"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh08, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh09 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 09"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh09, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh10 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 10"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh10, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh11 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 11"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh11, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionCh12 =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Channel 12"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionCh12, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mButtonIntersectionChEstimatedCell =
      new wxToggleButton(mPanelIntersectionButtons, wxID_ANY, _("Reco. Cell"), wxDefaultPosition, wxDefaultSize, 0);
  mGridIntersection->Add(mButtonIntersectionChEstimatedCell, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

  mPanelIntersectionButtons->SetSizer(mGridIntersection);
  mPanelIntersectionButtons->Layout();
  mGridIntersection->Fit(mPanelIntersectionButtons);
  mSizerIntersection->Add(mPanelIntersectionButtons, 0, wxALL | wxEXPAND, 5);

  mSizerIntersection->Add(0, 0, 1, wxEXPAND, 5);

  mGridIntersection->Add(mSizerIntersection, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

  mPanelIntersection->SetSizer(mGridIntersection);
  mPanelIntersection->Layout();
  mSizerPipelineSteps->Add(mPanelIntersection, 1, wxEXPAND | wxALL, 5);

  mPanelPipelineSteps->SetSizer(mSizerPipelineSteps);
  mPanelPipelineSteps->Layout();
  mSizerPipelineSteps->Fit(mPanelPipelineSteps);
  mNotebookMain->AddPage(mPanelPipelineSteps, _("Pipeline steps"), false);

  mSizerMain->Add(mNotebookMain, 1, wxEXPAND | wxALL, 5);

  this->SetSizer(mSizerMain);
  this->Layout();
  m_statusBar1 = this->CreateStatusBar(1, wxSTB_SIZEGRIP, wxID_ANY);

  this->Centre(wxBOTH);
}

frameMain::~frameMain()
{
}

dialogProcessing::dialogProcessing(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos,
                                   const wxSize &size, long style) :
    wxDialog(parent, id, title, pos, size, style)
{
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);
  this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));

  wxBoxSizer *mSizerProcessing;
  mSizerProcessing = new wxBoxSizer(wxVERTICAL);

  mSizerProcessing->Add(0, 0, 1, wxEXPAND, 5);

  mLabelProgressImage =
      new wxStaticText(this, wxID_ANY, _("0/0"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  mLabelProgressImage->Wrap(-1);
  mSizerProcessing->Add(mLabelProgressImage, 0, wxEXPAND | wxTOP, 5);

  mProgressImage = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
  mProgressImage->SetValue(0);
  mSizerProcessing->Add(mProgressImage, 0, wxALL | wxEXPAND, 5);

  mLabelProgressAllOver =
      new wxStaticText(this, wxID_ANY, _("0/0"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  mLabelProgressAllOver->Wrap(-1);
  mSizerProcessing->Add(mLabelProgressAllOver, 0, wxEXPAND | wxTOP, 5);

  mProgressAllOver = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
  mProgressAllOver->SetValue(0);
  mSizerProcessing->Add(mProgressAllOver, 0, wxALL | wxEXPAND, 5);

  mLineProgressDialog = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  mSizerProcessing->Add(mLineProgressDialog, 0, wxEXPAND | wxALL, 5);

  mPanelFooter = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer *mSizerFooter;
  mSizerFooter = new wxBoxSizer(wxHORIZONTAL);

  panelFooterButtons = new wxPanel(mPanelFooter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  panelFooterButtons->SetMaxSize(wxSize(-1, 65));

  wxBoxSizer *sizerFooterButtons;
  sizerFooterButtons = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizerFooterButtons2;
  sizerFooterButtons2 = new wxBoxSizer(wxHORIZONTAL);

  iconCpuCores = new wxStaticBitmap(panelFooterButtons, wxID_ANY, icons8 - cpu - 20_png_to_wx_bitmap(),
                                    wxDefaultPosition, wxDefaultSize, 0);
  iconCpuCores->SetMaxSize(wxSize(20, -1));

  sizerFooterButtons2->Add(iconCpuCores, 0, wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxLEFT | wxRIGHT,
                           5);

  mSpinCpuCores = new wxSpinCtrlDouble(panelFooterButtons, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                       wxSP_ARROW_KEYS, 1, 100, 1, 1);
  mSpinCpuCores->SetDigits(0);
  sizerFooterButtons2->Add(mSpinCpuCores, 1, wxALIGN_CENTER_VERTICAL, 5);

  mButtonStart = new wxButton(panelFooterButtons, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0);
  sizerFooterButtons2->Add(mButtonStart, 0, wxALIGN_CENTER | wxALL, 5);

  mButtonStop = new wxButton(panelFooterButtons, wxID_ANY, _("Stop"), wxDefaultPosition, wxDefaultSize, 0);
  sizerFooterButtons2->Add(mButtonStop, 0, wxALL, 5);

  sizerFooterButtons->Add(sizerFooterButtons2, 0, wxEXPAND | wxRIGHT | wxTOP, 5);

  mLabelCpuCores = new wxStaticText(panelFooterButtons, wxID_ANY, _("CPU cores to use"), wxDefaultPosition,
                                    wxDefaultSize, wxALIGN_LEFT);
  mLabelCpuCores->Wrap(-1);
  mLabelCpuCores->SetFont(
      wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

  sizerFooterButtons->Add(mLabelCpuCores, 0, wxALIGN_LEFT | wxALIGN_TOP | wxEXPAND | wxLEFT, 32);

  panelFooterButtons->SetSizer(sizerFooterButtons);
  panelFooterButtons->Layout();
  sizerFooterButtons->Fit(panelFooterButtons);
  mSizerFooter->Add(panelFooterButtons, 0, wxTOP, 5);

  mPanelFooter->SetSizer(mSizerFooter);
  mPanelFooter->Layout();
  mSizerFooter->Fit(mPanelFooter);
  mSizerProcessing->Add(mPanelFooter, 1, wxALIGN_RIGHT | wxALL, 5);

  this->SetSizer(mSizerProcessing);
  this->Layout();

  this->Centre(wxBOTH);
}

dialogProcessing::~dialogProcessing()
{
}

dialogAbout::dialogAbout(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size,
                         long style) :
    wxDialog(parent, id, title, pos, size, style)
{
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer *mSizerAbout;
  mSizerAbout = new wxBoxSizer(wxVERTICAL);

  mIconLogo = new wxStaticBitmap(this, wxID_ANY, imagec_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0);
  mSizerAbout->Add(mIconLogo, 0, wxALIGN_CENTER | wxALL, 5);

  mLabelTitle = new wxStaticText(this, wxID_ANY, _("<b>imageC 1.0.0</b>"), wxDefaultPosition, wxDefaultSize, 0);
  mLabelTitle->SetLabelMarkup(_("<b>imageC 1.0.0</b>"));
  mLabelTitle->Wrap(-1);
  mSizerAbout->Add(mLabelTitle, 0, wxALIGN_CENTER | wxALL, 5);

  mLabelDescription = new wxStaticText(
      this, wxID_ANY,
      _("Preferably for use in the non-profit research environment.\n\n<b>Many thanks to:</b> Melanie Schürz, Anna "
        "Müller, Tanja Plank, Maria Jartisch\n\n\nIcons from <i>https://icons8.com/</i>\n\n(c) 2023 Joachim Danmayr"),
      wxDefaultPosition, wxDefaultSize, 0);
  mLabelDescription->SetLabelMarkup(
      _("Preferably for use in the non-profit research environment.\n\n<b>Many thanks to:</b> Melanie Schürz, Anna "
        "Müller, Tanja Plank, Maria Jartisch\n\n\nIcons from <i>https://icons8.com/</i>\n\n(c) 2023 Joachim Danmayr"));
  mLabelDescription->Wrap(-1);
  mSizerAbout->Add(mLabelDescription, 0, wxALIGN_CENTER | wxALL, 5);

  this->SetSizer(mSizerAbout);
  this->Layout();

  this->Centre(wxBOTH);
}

dialogAbout::~dialogAbout()
{
}
