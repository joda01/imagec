///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3-dirty)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxwidget.h"

#include "../../res/about_logo.png.h"
#include "../../res/ai_20.png.h"
#include "../../res/all_out_20.png.h"
#include "../../res/background_remover_20.png.h"
#include "../../res/blur_20.png.h"
#include "../../res/bursts_20.png.h"
#include "../../res/centre_point_20.png.h"
#include "../../res/combine_20.png.h"
#include "../../res/contrast_20.png.h"
#include "../../res/cpu_20.png.h"
#include "../../res/crop_20.png.h"
#include "../../res/electrical_threshold_20.png.h"
#include "../../res/formula_fx_20.png.h"
#include "../../res/info_20.png.h"
#include "../../res/layers_20.png.h"
#include "../../res/octagon_20.png.h"
#include "../../res/opened_folder_20.png.h"
#include "../../res/plus_math_20.png.h"
#include "../../res/preview_20.png.h"
#include "../../res/ram_20.png.h"
#include "../../res/save_20.png.h"
#include "../../res/settings_20.png.h"
#include "../../res/smooth_20.png.h"
#include "../../res/sphere_20.png.h"
#include "../../res/start_20.png.h"

///////////////////////////////////////////////////////////////////////////
using namespace joda::ui::wxwidget;

frameMain::frameMain( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 1000,650 ), wxDefaultSize );

	mToolBar = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );
	mButtonSave = mToolBar->AddTool( wxID_ANY, _("Save"), save_20_png_to_wx_bitmap(), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	mButtonOpen = mToolBar->AddTool( wxID_ANY, _("Open"), opened_folder_20_png_to_wx_bitmap(), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	mToolBar->AddSeparator();

	mLabelNrOfFoundFiles = new wxStaticText( mToolBar, wxID_ANY, _("Images dir: "), wxDefaultPosition, wxSize( 140,-1 ), wxALIGN_LEFT );
	mLabelNrOfFoundFiles->Wrap( -1 );
	mToolBar->AddControl( mLabelNrOfFoundFiles );
	mDirectoryPicker = new wxDirPickerCtrl( mToolBar, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxSize( 250,-1 ), wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST );
	mToolBar->AddControl( mDirectoryPicker );
	mButtonRun = mToolBar->AddTool( wxID_ANY, _("Start"), start_20_png_to_wx_bitmap(), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	mToolBar->AddSeparator();

	mButtonSettings = mToolBar->AddTool( wxID_ANY, _("Settings"), settings_20_png_to_wx_bitmap(), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	mButtonAbout = mToolBar->AddTool( wxID_ANY, _("Info"), info_20_png_to_wx_bitmap(), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	mToolBar->Realize();

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	mNotebookMain = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	mPanelChannel = new wxPanel( mNotebookMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mSizerChannelsScrollbar = new wxBoxSizer( wxVERTICAL );

	mScrollbarChannels = new wxScrolledWindow( mPanelChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL );
	mScrollbarChannels->SetScrollRate( 5, 0 );
	mSizerChannels = new wxBoxSizer( wxHORIZONTAL );

	mButtonAddChannel = new wxButton( mScrollbarChannels, wxID_ANY, _("Add Channel"), wxDefaultPosition, wxDefaultSize, 0 );

	mButtonAddChannel->SetBitmap( plus_math_20_png_to_wx_bitmap() );
	mSizerChannels->Add( mButtonAddChannel, 0, wxALIGN_CENTER|wxALL, 5 );


	mScrollbarChannels->SetSizer( mSizerChannels );
	mScrollbarChannels->Layout();
	mSizerChannels->Fit( mScrollbarChannels );
	mSizerChannelsScrollbar->Add( mScrollbarChannels, 1, wxEXPAND|wxALL, 5 );


	mPanelChannel->SetSizer( mSizerChannelsScrollbar );
	mPanelChannel->Layout();
	mSizerChannelsScrollbar->Fit( mPanelChannel );
	mNotebookMain->AddPage( mPanelChannel, _("Channels"), true );
	mPanelPipelineSteps = new wxPanel( mNotebookMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mSizerHorizontalScrolPipelineSteps = new wxBoxSizer( wxVERTICAL );

	mScrrollbarPipelineStep = new wxScrolledWindow( mPanelPipelineSteps, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	mScrrollbarPipelineStep->SetScrollRate( 5, 0 );
	mSizerPipelineStep = new wxBoxSizer( wxHORIZONTAL );

	panelPipelineStepCellApproximation = new wxScrolledWindow( mScrrollbarPipelineStep, wxID_ANY, wxDefaultPosition, wxSize( 250,-1 ), wxBORDER_NONE|wxVSCROLL );
	panelPipelineStepCellApproximation->SetScrollRate( 5, 5 );
	panelPipelineStepCellApproximation->SetBackgroundColour( wxColour( 245, 245, 245 ) );
	panelPipelineStepCellApproximation->SetMaxSize( wxSize( 250,-1 ) );

	wxGridSizer* mGridCellEstimation;
	mGridCellEstimation = new wxGridSizer( 0, 1, 0, 0 );

	wxBoxSizer* mSizerCellEstimation;
	mSizerCellEstimation = new wxBoxSizer( wxVERTICAL );

	mLabelCellEstimation = new wxStaticText( panelPipelineStepCellApproximation, wxID_ANY, _("Cell approximation"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	mLabelCellEstimation->Wrap( -1 );
	mLabelCellEstimation->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	mSizerCellEstimation->Add( mLabelCellEstimation, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );

	panelNucleusChannel = new wxPanel( panelPipelineStepCellApproximation, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelNucleusChannel->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerNucleusChannel;
	sizerNucleusChannel = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerNucleusChannel2;
	sizerNucleusChannel2 = new wxBoxSizer( wxHORIZONTAL );

	iconNucluesChannel = new wxStaticBitmap( panelNucleusChannel, wxID_ANY, bursts_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerNucleusChannel2->Add( iconNucluesChannel, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mChoiceNucluesChannelChoices[] = { _("Off"), _("Channel 1"), _("Channel 2"), _("Channel 3"), _("Channel 4"), _("Channel 5"), _("Channel 6"), _("Channel 7"), _("Channel 8"), _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12") };
	int mChoiceNucluesChannelNChoices = sizeof( mChoiceNucluesChannelChoices ) / sizeof( wxString );
	mChoiceNucluesChannel = new wxChoice( panelNucleusChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceNucluesChannelNChoices, mChoiceNucluesChannelChoices, 0 );
	mChoiceNucluesChannel->SetSelection( 0 );
	sizerNucleusChannel2->Add( mChoiceNucluesChannel, 1, wxEXPAND, 5 );


	sizerNucleusChannel->Add( sizerNucleusChannel2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelNucleusChannel = new wxStaticText( panelNucleusChannel, wxID_ANY, _("Nucleus channel"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelNucleusChannel->Wrap( -1 );
	mLabelNucleusChannel->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerNucleusChannel->Add( mLabelNucleusChannel, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelNucleusChannel->SetSizer( sizerNucleusChannel );
	panelNucleusChannel->Layout();
	sizerNucleusChannel->Fit( panelNucleusChannel );
	mSizerCellEstimation->Add( panelNucleusChannel, 1, wxEXPAND|wxTOP, 5 );

	panelCellChannel = new wxPanel( panelPipelineStepCellApproximation, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelCellChannel->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerCellChannel;
	sizerCellChannel = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerCellChannel2;
	sizerCellChannel2 = new wxBoxSizer( wxHORIZONTAL );

	iconCellChannel = new wxStaticBitmap( panelCellChannel, wxID_ANY, bursts_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerCellChannel2->Add( iconCellChannel, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mChoiceCellChannelChoices[] = { _("Off"), _("Channel 1"), _("Channel 2"), _("Channel 3"), _("Channel 4"), _("Channel 5"), _("Channel 6"), _("Channel 7"), _("Channel 8"), _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12") };
	int mChoiceCellChannelNChoices = sizeof( mChoiceCellChannelChoices ) / sizeof( wxString );
	mChoiceCellChannel = new wxChoice( panelCellChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceCellChannelNChoices, mChoiceCellChannelChoices, 0 );
	mChoiceCellChannel->SetSelection( 0 );
	sizerCellChannel2->Add( mChoiceCellChannel, 1, wxEXPAND, 5 );


	sizerCellChannel->Add( sizerCellChannel2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelCellChannel = new wxStaticText( panelCellChannel, wxID_ANY, _("Cell channel"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelCellChannel->Wrap( -1 );
	mLabelCellChannel->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerCellChannel->Add( mLabelCellChannel, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelCellChannel->SetSizer( sizerCellChannel );
	panelCellChannel->Layout();
	sizerCellChannel->Fit( panelCellChannel );
	mSizerCellEstimation->Add( panelCellChannel, 1, wxEXPAND|wxTOP, 5 );

	panelMaxCellRadius = new wxPanel( panelPipelineStepCellApproximation, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelMaxCellRadius->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerMaxCellRadius;
	sizerMaxCellRadius = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerMaxCellRadius2;
	sizerMaxCellRadius2 = new wxBoxSizer( wxHORIZONTAL );

	iconMaxCellRadius = new wxStaticBitmap( panelMaxCellRadius, wxID_ANY, all_out_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerMaxCellRadius2->Add( iconMaxCellRadius, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mSpinMaxCellRadius = new wxSpinCtrl( panelMaxCellRadius, wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 256, 0.000000 );
	sizerMaxCellRadius2->Add( mSpinMaxCellRadius, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );


	sizerMaxCellRadius->Add( sizerMaxCellRadius2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelMaxCellRadius = new wxStaticText( panelMaxCellRadius, wxID_ANY, _("Maximum cell radius [px2]"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelMaxCellRadius->Wrap( -1 );
	mLabelMaxCellRadius->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerMaxCellRadius->Add( mLabelMaxCellRadius, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelMaxCellRadius->SetSizer( sizerMaxCellRadius );
	panelMaxCellRadius->Layout();
	sizerMaxCellRadius->Fit( panelMaxCellRadius );
	mSizerCellEstimation->Add( panelMaxCellRadius, 1, wxEXPAND|wxTOP, 5 );

	m_staticline7 = new wxStaticLine( panelPipelineStepCellApproximation, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mSizerCellEstimation->Add( m_staticline7, 0, wxBOTTOM|wxEXPAND|wxTOP, 15 );

	mLabelSpotRemoval = new wxStaticText( panelPipelineStepCellApproximation, wxID_ANY, _("Spot removal"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	mLabelSpotRemoval->Wrap( -1 );
	mLabelSpotRemoval->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	mSizerCellEstimation->Add( mLabelSpotRemoval, 0, wxALIGN_CENTER|wxALL|wxEXPAND|wxTOP, 5 );

	panelTetraspeckChannel = new wxPanel( panelPipelineStepCellApproximation, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelTetraspeckChannel->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerTetraspeckChannel;
	sizerTetraspeckChannel = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerTetraspeckChannel2;
	sizerTetraspeckChannel2 = new wxBoxSizer( wxHORIZONTAL );

	iconTetraspeckChannel = new wxStaticBitmap( panelTetraspeckChannel, wxID_ANY, bursts_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerTetraspeckChannel2->Add( iconTetraspeckChannel, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mChoiceTetraspeckChannelChoices[] = { _("Off"), _("Channel 1"), _("Channel 2"), _("Channel 3"), _("Channel 4"), _("Channel 5"), _("Channel 6"), _("Channel 7"), _("Channel 8"), _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12") };
	int mChoiceTetraspeckChannelNChoices = sizeof( mChoiceTetraspeckChannelChoices ) / sizeof( wxString );
	mChoiceTetraspeckChannel = new wxChoice( panelTetraspeckChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceTetraspeckChannelNChoices, mChoiceTetraspeckChannelChoices, 0 );
	mChoiceTetraspeckChannel->SetSelection( 0 );
	sizerTetraspeckChannel2->Add( mChoiceTetraspeckChannel, 1, wxEXPAND, 5 );


	sizerTetraspeckChannel->Add( sizerTetraspeckChannel2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelTetraspeckChannel = new wxStaticText( panelTetraspeckChannel, wxID_ANY, _("Tetraspeck Spot channel"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelTetraspeckChannel->Wrap( -1 );
	mLabelTetraspeckChannel->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerTetraspeckChannel->Add( mLabelTetraspeckChannel, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelTetraspeckChannel->SetSizer( sizerTetraspeckChannel );
	panelTetraspeckChannel->Layout();
	sizerTetraspeckChannel->Fit( panelTetraspeckChannel );
	mSizerCellEstimation->Add( panelTetraspeckChannel, 1, wxEXPAND, 5 );


	mGridCellEstimation->Add( mSizerCellEstimation, 1, wxEXPAND|wxLEFT|wxRIGHT, 10 );


	mGridCellEstimation->Add( 0, 0, 1, wxEXPAND, 5 );


	panelPipelineStepCellApproximation->SetSizer( mGridCellEstimation );
	panelPipelineStepCellApproximation->Layout();
	mSizerPipelineStep->Add( panelPipelineStepCellApproximation, 1, wxALL|wxEXPAND, 5 );

	mButtonAddIntersection = new wxButton( mScrrollbarPipelineStep, wxID_ANY, _("Add intersection step"), wxDefaultPosition, wxDefaultSize, 0 );

	mButtonAddIntersection->SetBitmap( plus_math_20_png_to_wx_bitmap() );
	mSizerPipelineStep->Add( mButtonAddIntersection, 0, wxALIGN_CENTER|wxALL, 5 );


	mScrrollbarPipelineStep->SetSizer( mSizerPipelineStep );
	mScrrollbarPipelineStep->Layout();
	mSizerPipelineStep->Fit( mScrrollbarPipelineStep );
	mSizerHorizontalScrolPipelineSteps->Add( mScrrollbarPipelineStep, 1, wxEXPAND | wxALL, 5 );


	mPanelPipelineSteps->SetSizer( mSizerHorizontalScrolPipelineSteps );
	mPanelPipelineSteps->Layout();
	mSizerHorizontalScrolPipelineSteps->Fit( mPanelPipelineSteps );
	mNotebookMain->AddPage( mPanelPipelineSteps, _("Pipeline steps"), false );

	bSizer1->Add( mNotebookMain, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	m_statusBar1 = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( mButtonSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( frameMain::onSaveSettingsClicked ) );
	this->Connect( mButtonOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( frameMain::onOpenSettingsClicked ) );
	mDirectoryPicker->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( frameMain::onWorkingDirChanged ), NULL, this );
	this->Connect( mButtonRun->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( frameMain::onRunClicked ) );
	this->Connect( mButtonAbout->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( frameMain::onAboutClicked ) );
	mButtonAddChannel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onAddChannelClicked ), NULL, this );
	mChoiceCellChannel->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( frameMain::onCellChannelChoice ), NULL, this );
	mButtonAddIntersection->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onAddIntersectionClicked ), NULL, this );
}

frameMain::~frameMain()
{
	// Disconnect Events
	this->Disconnect( mButtonSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( frameMain::onSaveSettingsClicked ) );
	this->Disconnect( mButtonOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( frameMain::onOpenSettingsClicked ) );
	mDirectoryPicker->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( frameMain::onWorkingDirChanged ), NULL, this );
	this->Disconnect( mButtonRun->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( frameMain::onRunClicked ) );
	this->Disconnect( mButtonAbout->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( frameMain::onAboutClicked ) );
	mButtonAddChannel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onAddChannelClicked ), NULL, this );
	mChoiceCellChannel->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( frameMain::onCellChannelChoice ), NULL, this );
	mButtonAddIntersection->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onAddIntersectionClicked ), NULL, this );

}

DialogProcessing::DialogProcessing( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxColour( 255, 255, 255 ) );

	wxBoxSizer* mSizerProcessing;
	mSizerProcessing = new wxBoxSizer( wxVERTICAL );

	mLabelProgressImage = new wxStaticText( this, wxID_ANY, _("0/0"), wxDefaultPosition, wxSize( 200,-1 ), wxALIGN_CENTER_HORIZONTAL );
	mLabelProgressImage->Wrap( -1 );
	mSizerProcessing->Add( mLabelProgressImage, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );

	mProgressImage = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	mProgressImage->SetValue( 0 );
	mSizerProcessing->Add( mProgressImage, 0, wxALL|wxEXPAND, 5 );

	mLabelProgressAllOver = new wxStaticText( this, wxID_ANY, _("0/0"), wxDefaultPosition, wxSize( 200,-1 ), wxALIGN_CENTER_HORIZONTAL );
	mLabelProgressAllOver->Wrap( -1 );
	mSizerProcessing->Add( mLabelProgressAllOver, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );

	mProgressAllOver = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	mProgressAllOver->SetValue( 0 );
	mSizerProcessing->Add( mProgressAllOver, 0, wxALL|wxEXPAND, 5 );


	mSizerProcessing->Add( 0, 0, 1, wxEXPAND, 5 );

	mLabelReporting = new wxStaticText( this, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	mLabelReporting->Wrap( -1 );
	mSizerProcessing->Add( mLabelReporting, 0, wxALL, 5 );

	mLineProgressDialog = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mSizerProcessing->Add( mLineProgressDialog, 0, wxEXPAND|wxALL, 5 );

	mPanelFooter = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* mSizerFooter;
	mSizerFooter = new wxBoxSizer( wxHORIZONTAL );

	panelFooterButtons = new wxPanel( mPanelFooter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizerFooterButtons;
	sizerFooterButtons = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerFooterButtons2;
	sizerFooterButtons2 = new wxBoxSizer( wxHORIZONTAL );

	m_bitmap21 = new wxStaticBitmap( panelFooterButtons, wxID_ANY, ram_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerFooterButtons2->Add( m_bitmap21, 0, wxALIGN_CENTER|wxALL, 5 );

	labelRAM = new wxStaticText( panelFooterButtons, wxID_ANY, _("10.10 GB / 10.10 MB / 10.10 MB"), wxDefaultPosition, wxSize( 250,-1 ), 0 );
	labelRAM->Wrap( -1 );
	labelRAM->SetToolTip( _("RAM per image / Free RAM / Available RAM") );

	sizerFooterButtons2->Add( labelRAM, 0, wxALIGN_CENTER|wxALL, 5 );

	iconCpuCores = new wxStaticBitmap( panelFooterButtons, wxID_ANY, cpu_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerFooterButtons2->Add( iconCpuCores, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	labelAvailableCores = new wxStaticText( panelFooterButtons, wxID_ANY, _("99 / 99 /  99 / 100"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	labelAvailableCores->Wrap( -1 );
	labelAvailableCores->SetToolTip( _("Cores per Image / Cores per Tile / Cores per Channel / Availabel cores") );

	sizerFooterButtons2->Add( labelAvailableCores, 0, wxALIGN_CENTER|wxALL, 5 );

	mButtonStop = new wxButton( panelFooterButtons, wxID_ANY, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerFooterButtons2->Add( mButtonStop, 0, wxALL, 5 );

	mButtonClose = new wxButton( panelFooterButtons, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerFooterButtons2->Add( mButtonClose, 0, wxALL, 5 );


	sizerFooterButtons->Add( sizerFooterButtons2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );


	panelFooterButtons->SetSizer( sizerFooterButtons );
	panelFooterButtons->Layout();
	sizerFooterButtons->Fit( panelFooterButtons );
	mSizerFooter->Add( panelFooterButtons, 0, wxTOP, 5 );


	mPanelFooter->SetSizer( mSizerFooter );
	mPanelFooter->Layout();
	mSizerFooter->Fit( mPanelFooter );
	mSizerProcessing->Add( mPanelFooter, 1, wxALIGN_RIGHT|wxALL, 5 );


	this->SetSizer( mSizerProcessing );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	mButtonStop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogProcessing::onStopClicked ), NULL, this );
	mButtonClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogProcessing::onCloseClicked ), NULL, this );
}

DialogProcessing::~DialogProcessing()
{
	// Disconnect Events
	mButtonStop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogProcessing::onStopClicked ), NULL, this );
	mButtonClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogProcessing::onCloseClicked ), NULL, this );

}

DialogAbout::DialogAbout( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mSizerAbout;
	mSizerAbout = new wxBoxSizer( wxVERTICAL );

	mIconLogo = new wxStaticBitmap( this, wxID_ANY, about_logo_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	mSizerAbout->Add( mIconLogo, 0, wxALIGN_CENTER|wxALL, 5 );

	mLabelTitle = new wxStaticText( this, wxID_ANY, _("<b>imageC 1.0.0-preview</b>\n<i>Licensed under GPL-v3</i>\n\nPreferably for use in the non-profit research environment.\n\n<b>Many thanks</b> for your help in setting up this project to Melanie Schuerz and Anna Mueller.\n\n<b>I would also like to thank</b>\nMelanie Schuerz, Anna Mueller, Tanja Plank, Maria Jartisch, Heloisa Melobenirschke and Patricia Hrasnova for their support in AI training.\n\nIcons from <i>https://icons8.com/</i>\n\n<i>copyright 2023 Joachim Danmayr</i>\n\n...\n\n"), wxDefaultPosition, wxDefaultSize, 0 );
	mLabelTitle->SetLabelMarkup( _("<b>imageC 1.0.0-preview</b>\n<i>Licensed under GPL-v3</i>\n\nPreferably for use in the non-profit research environment.\n\n<b>Many thanks</b> for your help in setting up this project to Melanie Schuerz and Anna Mueller.\n\n<b>I would also like to thank</b>\nMelanie Schuerz, Anna Mueller, Tanja Plank, Maria Jartisch, Heloisa Melobenirschke and Patricia Hrasnova for their support in AI training.\n\nIcons from <i>https://icons8.com/</i>\n\n<i>copyright 2023 Joachim Danmayr</i>\n\n...\n\n") );
	mLabelTitle->Wrap( -1 );
	mSizerAbout->Add( mLabelTitle, 0, wxALIGN_CENTER|wxALL, 5 );


	mSizerAbout->Add( 0, 0, 1, wxEXPAND, 5 );


	this->SetSizer( mSizerAbout );
	this->Layout();

	this->Centre( wxBOTH );
}

DialogAbout::~DialogAbout()
{
}

PanelChannel::PanelChannel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	mSizerForScroll = new wxBoxSizer( wxHORIZONTAL );

	mSizerForScroll->SetMinSize( wxSize( 250,-1 ) );
	mScrolledChannel = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxSize( 250,-1 ), wxBORDER_NONE|wxVSCROLL );
	mScrolledChannel->SetScrollRate( 0, 5 );
	mScrolledChannel->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxGridSizer* mSizerGridChannel;
	mSizerGridChannel = new wxGridSizer( 0, 1, 0, 0 );

	wxBoxSizer* mSizerChannel;
	mSizerChannel = new wxBoxSizer( wxVERTICAL );

	mLabelChannelTitle = new wxStaticText( mScrolledChannel, wxID_ANY, _("Channel"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	mLabelChannelTitle->Wrap( -1 );
	mLabelChannelTitle->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	mSizerChannel->Add( mLabelChannelTitle, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );

	mTextChannelName = new wxTextCtrl( mScrolledChannel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mSizerChannel->Add( mTextChannelName, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	panelChannelType = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	panelChannelType->SetMaxSize( wxSize( -1,35 ) );

	wxBoxSizer* sizerChannelType;
	sizerChannelType = new wxBoxSizer( wxVERTICAL );

	wxString mChoiceChannelTypeChoices[] = { _("Spot"), _("Nucleus"), _("Cell"), _("Background") };
	int mChoiceChannelTypeNChoices = sizeof( mChoiceChannelTypeChoices ) / sizeof( wxString );
	mChoiceChannelType = new wxChoice( panelChannelType, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceChannelTypeNChoices, mChoiceChannelTypeChoices, 0 );
	mChoiceChannelType->SetSelection( 0 );
	sizerChannelType->Add( mChoiceChannelType, 0, wxEXPAND, 5 );


	panelChannelType->SetSizer( sizerChannelType );
	panelChannelType->Layout();
	sizerChannelType->Fit( panelChannelType );
	mSizerChannel->Add( panelChannelType, 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	panelChannelIndex = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	panelChannelIndex->SetMaxSize( wxSize( -1,35 ) );

	wxBoxSizer* sizerChannelIndex;
	sizerChannelIndex = new wxBoxSizer( wxVERTICAL );

	wxString mChoiceChannelIndexChoices[] = { _("Channel 1"), _("Channel 2"), _("Channel 3"), _("Channel 4"), _("Channel 5"), _("Channel 6"), _("Channel 7"), _("Channel 8"), _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12") };
	int mChoiceChannelIndexNChoices = sizeof( mChoiceChannelIndexChoices ) / sizeof( wxString );
	mChoiceChannelIndex = new wxChoice( panelChannelIndex, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceChannelIndexNChoices, mChoiceChannelIndexChoices, 0 );
	mChoiceChannelIndex->SetSelection( 0 );
	sizerChannelIndex->Add( mChoiceChannelIndex, 0, wxEXPAND, 5 );


	panelChannelIndex->SetSizer( sizerChannelIndex );
	panelChannelIndex->Layout();
	sizerChannelIndex->Fit( panelChannelIndex );
	mSizerChannel->Add( panelChannelIndex, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mLabelPreprocessing = new wxStaticText( mScrolledChannel, wxID_ANY, _("Preprocessing"), wxDefaultPosition, wxDefaultSize, 0 );
	mLabelPreprocessing->Wrap( -1 );
	mLabelPreprocessing->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	mSizerChannel->Add( mLabelPreprocessing, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mLinePreprocessing = new wxStaticLine( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mLinePreprocessing->SetBackgroundColour( wxColour( 0, 0, 0 ) );

	mSizerChannel->Add( mLinePreprocessing, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	panelZStack = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelZStack->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerZStack;
	sizerZStack = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerZStack2;
	sizerZStack2 = new wxBoxSizer( wxHORIZONTAL );

	iconZStack = new wxStaticBitmap( panelZStack, wxID_ANY, layers_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerZStack2->Add( iconZStack, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mChoiceZStackChoices[] = { _("Off"), _("Max. intensity projection"), _("3D projection") };
	int mChoiceZStackNChoices = sizeof( mChoiceZStackChoices ) / sizeof( wxString );
	mChoiceZStack = new wxChoice( panelZStack, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceZStackNChoices, mChoiceZStackChoices, 0 );
	mChoiceZStack->SetSelection( 0 );
	sizerZStack2->Add( mChoiceZStack, 1, wxEXPAND, 5 );


	sizerZStack->Add( sizerZStack2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelZStack = new wxStaticText( panelZStack, wxID_ANY, _("Z-stack projection"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelZStack->Wrap( -1 );
	mLabelZStack->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerZStack->Add( mLabelZStack, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelZStack->SetSizer( sizerZStack );
	panelZStack->Layout();
	sizerZStack->Fit( panelZStack );
	mSizerChannel->Add( panelZStack, 1, wxEXPAND|wxTOP, 5 );

	panelMarginCrop = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelMarginCrop->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerMarginCrop;
	sizerMarginCrop = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerMarginCrop2;
	sizerMarginCrop2 = new wxBoxSizer( wxHORIZONTAL );

	iconMarginCrop = new wxStaticBitmap( panelMarginCrop, wxID_ANY, crop_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerMarginCrop2->Add( iconMarginCrop, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mSpinMarginCrop = new wxSpinCtrl( panelMarginCrop, wxID_ANY, wxT("0.000000"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0.000000 );
	sizerMarginCrop2->Add( mSpinMarginCrop, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );


	sizerMarginCrop->Add( sizerMarginCrop2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelMarginCrop = new wxStaticText( panelMarginCrop, wxID_ANY, _("Margin crop [px]"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelMarginCrop->Wrap( -1 );
	mLabelMarginCrop->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerMarginCrop->Add( mLabelMarginCrop, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelMarginCrop->SetSizer( sizerMarginCrop );
	panelMarginCrop->Layout();
	sizerMarginCrop->Fit( panelMarginCrop );
	mSizerChannel->Add( panelMarginCrop, 1, wxEXPAND|wxTOP, 5 );

	panelMedianBGSubtract = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelMedianBGSubtract->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerMedianBGSubtract;
	sizerMedianBGSubtract = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerMedianBGSubtract2;
	sizerMedianBGSubtract2 = new wxBoxSizer( wxHORIZONTAL );

	iconMedianBGSubtract = new wxStaticBitmap( panelMedianBGSubtract, wxID_ANY, electrical_threshold_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerMedianBGSubtract2->Add( iconMedianBGSubtract, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mChoiceMedianBGSubtractChoices[] = { _("Off"), _("On") };
	int mChoiceMedianBGSubtractNChoices = sizeof( mChoiceMedianBGSubtractChoices ) / sizeof( wxString );
	mChoiceMedianBGSubtract = new wxChoice( panelMedianBGSubtract, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceMedianBGSubtractNChoices, mChoiceMedianBGSubtractChoices, 0 );
	mChoiceMedianBGSubtract->SetSelection( 0 );
	sizerMedianBGSubtract2->Add( mChoiceMedianBGSubtract, 1, wxEXPAND, 5 );


	sizerMedianBGSubtract->Add( sizerMedianBGSubtract2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelMedianBGSubtract = new wxStaticText( panelMedianBGSubtract, wxID_ANY, _("Median based BG substraction"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelMedianBGSubtract->Wrap( -1 );
	mLabelMedianBGSubtract->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerMedianBGSubtract->Add( mLabelMedianBGSubtract, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelMedianBGSubtract->SetSizer( sizerMedianBGSubtract );
	panelMedianBGSubtract->Layout();
	sizerMedianBGSubtract->Fit( panelMedianBGSubtract );
	mSizerChannel->Add( panelMedianBGSubtract, 1, wxEXPAND|wxTOP, 5 );

	panelRollingBall = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelRollingBall->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerRollingBall;
	sizerRollingBall = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerRollingBall2;
	sizerRollingBall2 = new wxBoxSizer( wxHORIZONTAL );

	iconRollingBall = new wxStaticBitmap( panelRollingBall, wxID_ANY, sphere_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerRollingBall2->Add( iconRollingBall, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mSpinRollingBall = new wxSpinCtrl( panelRollingBall, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 256, 0 );
	sizerRollingBall2->Add( mSpinRollingBall, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );


	sizerRollingBall->Add( sizerRollingBall2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelRollingBall = new wxStaticText( panelRollingBall, wxID_ANY, _("Rolling Ball BG substraction"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelRollingBall->Wrap( -1 );
	mLabelRollingBall->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerRollingBall->Add( mLabelRollingBall, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelRollingBall->SetSizer( sizerRollingBall );
	panelRollingBall->Layout();
	sizerRollingBall->Fit( panelRollingBall );
	mSizerChannel->Add( panelRollingBall, 1, wxEXPAND|wxTOP, 5 );

	panelBGSubtraction = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelBGSubtraction->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerBGSubtraction;
	sizerBGSubtraction = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerBGSubtraction2;
	sizerBGSubtraction2 = new wxBoxSizer( wxHORIZONTAL );

	iconBGSubtraction = new wxStaticBitmap( panelBGSubtraction, wxID_ANY, background_remover_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerBGSubtraction2->Add( iconBGSubtraction, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mChoiceBGSubtractionChoices[] = { _("Off"), _("Channel 1"), _("Channel 2"), _("Channel 3"), _("Channel 4"), _("Channel 5"), _("Channel 6"), _("Channel 7"), _("Channel 8"), _("Channel 9"), _("Channel 10"), _("Channel 11"), _("Channel 12") };
	int mChoiceBGSubtractionNChoices = sizeof( mChoiceBGSubtractionChoices ) / sizeof( wxString );
	mChoiceBGSubtraction = new wxChoice( panelBGSubtraction, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceBGSubtractionNChoices, mChoiceBGSubtractionChoices, 0 );
	mChoiceBGSubtraction->SetSelection( 0 );
	sizerBGSubtraction2->Add( mChoiceBGSubtraction, 1, wxEXPAND, 5 );


	sizerBGSubtraction->Add( sizerBGSubtraction2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelBGSubtraction = new wxStaticText( panelBGSubtraction, wxID_ANY, _("Channel based BG substraction"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelBGSubtraction->Wrap( -1 );
	mLabelBGSubtraction->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerBGSubtraction->Add( mLabelBGSubtraction, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelBGSubtraction->SetSizer( sizerBGSubtraction );
	panelBGSubtraction->Layout();
	sizerBGSubtraction->Fit( panelBGSubtraction );
	mSizerChannel->Add( panelBGSubtraction, 1, wxEXPAND|wxTOP, 5 );

	panelSmoothing = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelSmoothing->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerSmoothing;
	sizerSmoothing = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerSmoothing2;
	sizerSmoothing2 = new wxBoxSizer( wxHORIZONTAL );

	iconSmoothing = new wxStaticBitmap( panelSmoothing, wxID_ANY, smooth_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerSmoothing2->Add( iconSmoothing, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mDropDownSmoothingRepeatChoices[] = { _("Off"), _("x1"), _("x2"), _("x3"), _("x4") };
	int mDropDownSmoothingRepeatNChoices = sizeof( mDropDownSmoothingRepeatChoices ) / sizeof( wxString );
	mDropDownSmoothingRepeat = new wxChoice( panelSmoothing, wxID_ANY, wxDefaultPosition, wxDefaultSize, mDropDownSmoothingRepeatNChoices, mDropDownSmoothingRepeatChoices, 0 );
	mDropDownSmoothingRepeat->SetSelection( 0 );
	sizerSmoothing2->Add( mDropDownSmoothingRepeat, 1, wxEXPAND, 5 );


	sizerSmoothing->Add( sizerSmoothing2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelSmoothing = new wxStaticText( panelSmoothing, wxID_ANY, _("Smoothing"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelSmoothing->Wrap( -1 );
	mLabelSmoothing->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerSmoothing->Add( mLabelSmoothing, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelSmoothing->SetSizer( sizerSmoothing );
	panelSmoothing->Layout();
	sizerSmoothing->Fit( panelSmoothing );
	mSizerChannel->Add( panelSmoothing, 1, wxEXPAND|wxTOP, 5 );

	panelGausianBlur = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelGausianBlur->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerGausianBlur;
	sizerGausianBlur = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerGausianBlur2;
	sizerGausianBlur2 = new wxBoxSizer( wxHORIZONTAL );

	iconGausianBluer = new wxStaticBitmap( panelGausianBlur, wxID_ANY, blur_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGausianBlur2->Add( iconGausianBluer, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mDropdownGausianBlurChoices[] = { _("Off"), _("3x3"), _("5x5"), _("7x7"), _("9x9"), _("11x11") };
	int mDropdownGausianBlurNChoices = sizeof( mDropdownGausianBlurChoices ) / sizeof( wxString );
	mDropdownGausianBlur = new wxChoice( panelGausianBlur, wxID_ANY, wxDefaultPosition, wxDefaultSize, mDropdownGausianBlurNChoices, mDropdownGausianBlurChoices, 0 );
	mDropdownGausianBlur->SetSelection( 0 );
	sizerGausianBlur2->Add( mDropdownGausianBlur, 1, wxEXPAND, 5 );

	wxString mDropDownGausianBlurRepeatChoices[] = { _("x1"), _("x2"), _("x3"), _("x4") };
	int mDropDownGausianBlurRepeatNChoices = sizeof( mDropDownGausianBlurRepeatChoices ) / sizeof( wxString );
	mDropDownGausianBlurRepeat = new wxChoice( panelGausianBlur, wxID_ANY, wxDefaultPosition, wxDefaultSize, mDropDownGausianBlurRepeatNChoices, mDropDownGausianBlurRepeatChoices, 0 );
	mDropDownGausianBlurRepeat->SetSelection( 0 );
	sizerGausianBlur2->Add( mDropDownGausianBlurRepeat, 0, wxEXPAND|wxLEFT, 5 );


	sizerGausianBlur->Add( sizerGausianBlur2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelGausianBluer = new wxStaticText( panelGausianBlur, wxID_ANY, _("Gaussian Blur"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelGausianBluer->Wrap( -1 );
	mLabelGausianBluer->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerGausianBlur->Add( mLabelGausianBluer, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelGausianBlur->SetSizer( sizerGausianBlur );
	panelGausianBlur->Layout();
	sizerGausianBlur->Fit( panelGausianBlur );
	mSizerChannel->Add( panelGausianBlur, 1, wxEXPAND|wxTOP, 5 );

	mLabelDescription = new wxStaticText( mScrolledChannel, wxID_ANY, _("Detection"), wxDefaultPosition, wxDefaultSize, 0 );
	mLabelDescription->Wrap( -1 );
	mLabelDescription->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	mSizerChannel->Add( mLabelDescription, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mLineDescription = new wxStaticLine( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mLineDescription->SetBackgroundColour( wxColour( 0, 0, 0 ) );

	mSizerChannel->Add( mLineDescription, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	panelUseAI = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelUseAI->SetMaxSize( wxSize( -1,35 ) );

	wxBoxSizer* sizerUseAI;
	sizerUseAI = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerUseAI2;
	sizerUseAI2 = new wxBoxSizer( wxHORIZONTAL );

	iconUseAI = new wxStaticBitmap( panelUseAI, wxID_ANY, ai_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerUseAI2->Add( iconUseAI, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mCheckUseAI = new wxCheckBox( panelUseAI, wxID_ANY, _("Use AI"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerUseAI2->Add( mCheckUseAI, 0, wxEXPAND, 5 );


	sizerUseAI->Add( sizerUseAI2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );


	panelUseAI->SetSizer( sizerUseAI );
	panelUseAI->Layout();
	sizerUseAI->Fit( panelUseAI );
	mSizerChannel->Add( panelUseAI, 1, wxEXPAND|wxTOP, 5 );

	panelThresholdMethod = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelThresholdMethod->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* ThresholdMethod;
	ThresholdMethod = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* izerThresholdMethod2;
	izerThresholdMethod2 = new wxBoxSizer( wxHORIZONTAL );

	iconThresholdMethod = new wxStaticBitmap( panelThresholdMethod, wxID_ANY, formula_fx_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	izerThresholdMethod2->Add( iconThresholdMethod, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	wxString mChoiceThresholdMethodChoices[] = { _("Manual"), _("Li"), _("Min. error"), _("Triangle") };
	int mChoiceThresholdMethodNChoices = sizeof( mChoiceThresholdMethodChoices ) / sizeof( wxString );
	mChoiceThresholdMethod = new wxChoice( panelThresholdMethod, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceThresholdMethodNChoices, mChoiceThresholdMethodChoices, 0 );
	mChoiceThresholdMethod->SetSelection( 2 );
	izerThresholdMethod2->Add( mChoiceThresholdMethod, 1, wxEXPAND, 5 );


	ThresholdMethod->Add( izerThresholdMethod2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelThresholdMethod = new wxStaticText( panelThresholdMethod, wxID_ANY, _("Threshold method"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelThresholdMethod->Wrap( -1 );
	mLabelThresholdMethod->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	ThresholdMethod->Add( mLabelThresholdMethod, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelThresholdMethod->SetSizer( ThresholdMethod );
	panelThresholdMethod->Layout();
	ThresholdMethod->Fit( panelThresholdMethod );
	mSizerChannel->Add( panelThresholdMethod, 1, wxEXPAND|wxTOP, 5 );

	panelMinThreshold = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelMinThreshold->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerMinThreshold;
	sizerMinThreshold = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerMinThreshold2;
	sizerMinThreshold2 = new wxBoxSizer( wxHORIZONTAL );

	iconMinThreshold = new wxStaticBitmap( panelMinThreshold, wxID_ANY, contrast_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerMinThreshold2->Add( iconMinThreshold, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mSpinMinThreshold = new wxSpinCtrl( panelMinThreshold, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	sizerMinThreshold2->Add( mSpinMinThreshold, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );


	sizerMinThreshold->Add( sizerMinThreshold2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelMinThreshold = new wxStaticText( panelMinThreshold, wxID_ANY, _("Min. Threshold [0-65535)"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelMinThreshold->Wrap( -1 );
	mLabelMinThreshold->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerMinThreshold->Add( mLabelMinThreshold, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelMinThreshold->SetSizer( sizerMinThreshold );
	panelMinThreshold->Layout();
	sizerMinThreshold->Fit( panelMinThreshold );
	mSizerChannel->Add( panelMinThreshold, 1, wxEXPAND|wxTOP, 5 );

	mLabelFilter = new wxStaticText( mScrolledChannel, wxID_ANY, _("Filtering"), wxDefaultPosition, wxDefaultSize, 0 );
	mLabelFilter->Wrap( -1 );
	mLabelFilter->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	mSizerChannel->Add( mLabelFilter, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mLineFilter = new wxStaticLine( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mLineFilter->SetForegroundColour( wxColour( 255, 255, 255 ) );
	mLineFilter->SetBackgroundColour( wxColour( 0, 0, 0 ) );

	mSizerChannel->Add( mLineFilter, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	panelMinCircularity = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelMinCircularity->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerMinCircularity;
	sizerMinCircularity = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerMinCircularity2;
	sizerMinCircularity2 = new wxBoxSizer( wxHORIZONTAL );

	iconMinCircularity = new wxStaticBitmap( panelMinCircularity, wxID_ANY, octagon_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerMinCircularity2->Add( iconMinCircularity, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mSpinMinCircularity = new wxSpinCtrlDouble( panelMinCircularity, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.000000, 0.01 );
	mSpinMinCircularity->SetDigits( 2 );
	sizerMinCircularity2->Add( mSpinMinCircularity, 1, wxEXPAND, 5 );


	sizerMinCircularity->Add( sizerMinCircularity2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelMinCircularity = new wxStaticText( panelMinCircularity, wxID_ANY, _("Min. circulartiy [%]"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelMinCircularity->Wrap( -1 );
	mLabelMinCircularity->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerMinCircularity->Add( mLabelMinCircularity, 0, wxEXPAND|wxLEFT, 32 );


	panelMinCircularity->SetSizer( sizerMinCircularity );
	panelMinCircularity->Layout();
	sizerMinCircularity->Fit( panelMinCircularity );
	mSizerChannel->Add( panelMinCircularity, 1, wxEXPAND|wxTOP, 5 );

	panelParticleSize = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelParticleSize->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerParticleSize;
	sizerParticleSize = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerParticleSize2;
	sizerParticleSize2 = new wxBoxSizer( wxHORIZONTAL );

	iDescription1121 = new wxStaticBitmap( panelParticleSize, wxID_ANY, all_out_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerParticleSize2->Add( iDescription1121, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mTextParticleSizeRange = new wxTextCtrl( panelParticleSize, wxID_ANY, _("5-9999"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerParticleSize2->Add( mTextParticleSizeRange, 1, wxEXPAND, 5 );


	sizerParticleSize->Add( sizerParticleSize2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelParticleSize = new wxStaticText( panelParticleSize, wxID_ANY, _("Particle size range [px2]"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelParticleSize->Wrap( -1 );
	mLabelParticleSize->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerParticleSize->Add( mLabelParticleSize, 0, wxEXPAND|wxLEFT, 32 );


	panelParticleSize->SetSizer( sizerParticleSize );
	panelParticleSize->Layout();
	sizerParticleSize->Fit( panelParticleSize );
	mSizerChannel->Add( panelParticleSize, 1, wxEXPAND|wxTOP, 5 );

	panelSnapArea = new wxPanel( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelSnapArea->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerSnapArea1;
	sizerSnapArea1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerSnapArea2;
	sizerSnapArea2 = new wxBoxSizer( wxHORIZONTAL );

	iconParticleSize = new wxStaticBitmap( panelSnapArea, wxID_ANY, centre_point_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerSnapArea2->Add( iconParticleSize, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mSpinSnapArea = new wxSpinCtrl( panelSnapArea, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 256, 0 );
	sizerSnapArea2->Add( mSpinSnapArea, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );


	sizerSnapArea1->Add( sizerSnapArea2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelSnapArea = new wxStaticText( panelSnapArea, wxID_ANY, _("Snap area diamater [px2]"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelSnapArea->Wrap( -1 );
	mLabelSnapArea->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerSnapArea1->Add( mLabelSnapArea, 0, wxEXPAND|wxLEFT, 32 );


	panelSnapArea->SetSizer( sizerSnapArea1 );
	panelSnapArea->Layout();
	sizerSnapArea1->Fit( panelSnapArea );
	mSizerChannel->Add( panelSnapArea, 1, wxEXPAND|wxTOP, 5 );

	mLinePreview = new wxStaticLine( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mLinePreview->SetForegroundColour( wxColour( 255, 255, 255 ) );
	mLinePreview->SetBackgroundColour( wxColour( 0, 0, 0 ) );

	mSizerChannel->Add( mLinePreview, 0, wxEXPAND|wxALL, 5 );

	mButtonPreview = new wxButton( mScrolledChannel, wxID_ANY, _("Preview"), wxDefaultPosition, wxDefaultSize, 0 );

	mButtonPreview->SetBitmap( preview_20_png_to_wx_bitmap() );
	mSizerChannel->Add( mButtonPreview, 0, wxALIGN_CENTER|wxALL, 5 );

	mLineRemove = new wxStaticLine( mScrolledChannel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mLineRemove->SetForegroundColour( wxColour( 255, 255, 255 ) );
	mLineRemove->SetBackgroundColour( wxColour( 0, 0, 0 ) );

	mSizerChannel->Add( mLineRemove, 0, wxEXPAND|wxALL, 5 );

	mButtonRemoveChannel = new wxButton( mScrolledChannel, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	mSizerChannel->Add( mButtonRemoveChannel, 0, wxALIGN_CENTER|wxALL, 5 );


	mSizerChannel->Add( 0, 0, 1, wxEXPAND|wxSHAPED, 5 );


	mSizerGridChannel->Add( mSizerChannel, 1, wxEXPAND|wxLEFT|wxRIGHT, 10 );


	mScrolledChannel->SetSizer( mSizerGridChannel );
	mScrolledChannel->Layout();
	mSizerForScroll->Add( mScrolledChannel, 0, wxEXPAND|wxALL, 5 );


	this->SetSizer( mSizerForScroll );
	this->Layout();

	// Connect Events
	mButtonPreview->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PanelChannel::onPreviewClicked ), NULL, this );
	mButtonRemoveChannel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PanelChannel::onRemoveClicked ), NULL, this );
}

PanelChannel::~PanelChannel()
{
	// Disconnect Events
	mButtonPreview->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PanelChannel::onPreviewClicked ), NULL, this );
	mButtonRemoveChannel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PanelChannel::onRemoveClicked ), NULL, this );

}

PanelIntersection::PanelIntersection( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* mSizerForScroll;
	mSizerForScroll = new wxBoxSizer( wxHORIZONTAL );

	mSizerForScroll->SetMinSize( wxSize( 250,-1 ) );
	mPanelIntersection = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxSize( 250,-1 ), wxBORDER_NONE|wxVSCROLL );
	mPanelIntersection->SetScrollRate( 5, 5 );
	mPanelIntersection->SetBackgroundColour( wxColour( 245, 245, 245 ) );
	mPanelIntersection->SetMaxSize( wxSize( 250,-1 ) );

	wxGridSizer* mGridIntersection2;
	mGridIntersection2 = new wxGridSizer( 0, 1, 0, 0 );

	wxBoxSizer* mSizerIntersection;
	mSizerIntersection = new wxBoxSizer( wxVERTICAL );

	mLabelIntersection = new wxStaticText( mPanelIntersection, wxID_ANY, _("Intersection"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	mLabelIntersection->Wrap( -1 );
	mLabelIntersection->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	mSizerIntersection->Add( mLabelIntersection, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );

	panelMinIntersection = new wxPanel( mPanelIntersection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	panelMinIntersection->SetMaxSize( wxSize( -1,65 ) );

	wxBoxSizer* sizerMinIntersection;
	sizerMinIntersection = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerMinIntersection2;
	sizerMinIntersection2 = new wxBoxSizer( wxHORIZONTAL );

	iconMinIntersection = new wxStaticBitmap( panelMinIntersection, wxID_ANY, combine_20_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	sizerMinIntersection2->Add( iconMinIntersection, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT|wxRIGHT, 5 );

	mSpinMinIntersection = new wxSpinCtrlDouble( panelMinIntersection, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.8, 0.01 );
	mSpinMinIntersection->SetDigits( 2 );
	sizerMinIntersection2->Add( mSpinMinIntersection, 1, 0, 5 );


	sizerMinIntersection->Add( sizerMinIntersection2, 0, wxEXPAND|wxRIGHT|wxTOP, 5 );

	mLabelMinIntersection = new wxStaticText( panelMinIntersection, wxID_ANY, _("Min. intersection"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mLabelMinIntersection->Wrap( -1 );
	mLabelMinIntersection->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerMinIntersection->Add( mLabelMinIntersection, 0, wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND|wxLEFT, 32 );


	panelMinIntersection->SetSizer( sizerMinIntersection );
	panelMinIntersection->Layout();
	sizerMinIntersection->Fit( panelMinIntersection );
	mSizerIntersection->Add( panelMinIntersection, 1, wxEXPAND|wxTOP, 5 );

	mPanelIntersectionButtons = new wxPanel( mPanelIntersection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL );
	wxGridSizer* mGridIntersection;
	mGridIntersection = new wxGridSizer( 0, 2, 0, 0 );

	mButtonIntersectionCh01 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 01"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh01, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh02 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 02"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh02, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh03 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 03"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh03, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh04 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 04"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh04, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh05 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 05"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh05, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh06 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 06"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh06, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh07 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 07"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh07, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh08 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 08"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh08, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh09 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 09"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh09, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh10 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 10"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh10, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh11 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 11"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh11, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionCh12 = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Channel 12"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionCh12, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	mButtonIntersectionChEstimatedCell = new wxToggleButton( mPanelIntersectionButtons, wxID_ANY, _("Approx Cells"), wxDefaultPosition, wxDefaultSize, 0 );
	mGridIntersection->Add( mButtonIntersectionChEstimatedCell, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );


	mPanelIntersectionButtons->SetSizer( mGridIntersection );
	mPanelIntersectionButtons->Layout();
	mGridIntersection->Fit( mPanelIntersectionButtons );
	mSizerIntersection->Add( mPanelIntersectionButtons, 0, wxEXPAND|wxTOP, 5 );

	m_staticline8 = new wxStaticLine( mPanelIntersection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mSizerIntersection->Add( m_staticline8, 0, wxEXPAND | wxALL, 5 );

	mButtonRemovePipelineStep = new wxButton( mPanelIntersection, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	mSizerIntersection->Add( mButtonRemovePipelineStep, 0, wxALIGN_CENTER|wxALL, 5 );


	mSizerIntersection->Add( 0, 0, 1, wxEXPAND, 5 );


	mGridIntersection2->Add( mSizerIntersection, 0, wxEXPAND|wxLEFT|wxRIGHT, 10 );


	mPanelIntersection->SetSizer( mGridIntersection2 );
	mPanelIntersection->Layout();
	mSizerForScroll->Add( mPanelIntersection, 1, wxEXPAND|wxALL, 5 );


	this->SetSizer( mSizerForScroll );
	this->Layout();
	mSizerForScroll->Fit( this );

	// Connect Events
	mButtonRemovePipelineStep->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PanelIntersection::onRemoveClicked ), NULL, this );
}

PanelIntersection::~PanelIntersection()
{
	// Disconnect Events
	mButtonRemovePipelineStep->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PanelIntersection::onRemoveClicked ), NULL, this );

}

DialogImage::DialogImage( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );


	this->Centre( wxBOTH );
}

DialogImage::~DialogImage()
{
}
