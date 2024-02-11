///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3-dirty)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/choice.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/statline.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/collpane.h>
#include <wx/checkbox.h>
#include <wx/tglbtn.h>

///////////////////////////////////////////////////////////////////////////

namespace joda
{
	namespace ui
	{
		namespace wxwidget
		{
			///////////////////////////////////////////////////////////////////////////////
			/// Class frameMain
			///////////////////////////////////////////////////////////////////////////////
			class frameMain : public wxFrame
			{
				private:

				protected:
					wxToolBar* mToolBar;
					wxToolBarToolBase* mButtonSave;
					wxToolBarToolBase* mButtonOpen;
					wxStaticText* mLabelNrOfFoundFiles;
					wxDirPickerCtrl* mDirectoryPicker;
					wxToolBarToolBase* mButtonRun;
					wxToolBarToolBase* mButtonSettings;
					wxToolBarToolBase* mButtonAbout;
					wxToolBar* mToolBarVertical;
					wxStaticText* mTitleChannel;
					wxToolBarToolBase* mButtonAddChannel;
					wxStaticText* mTitlePipelineSteps;
					wxToolBarToolBase* mButtonAddCellApproximation;
					wxToolBarToolBase* mButtonAddIntersection;
					wxNotebook* mNotebookMain;
					wxPanel* mPanelChannel;
					wxBoxSizer* mSizerChannelsScrollbar;
					wxScrolledWindow* mScrollbarChannels;
					wxBoxSizer* mSizerChannels;
					wxPanel* mPanelPipelineSteps;
					wxBoxSizer* mSizerHorizontalScrolPipelineSteps;
					wxScrolledWindow* mScrrollbarPipelineStep;
					wxBoxSizer* mSizerPipelineStep;
					wxStatusBar* mStatusBar;

					// Virtual event handlers, override them in your derived class
					virtual void onSaveSettingsClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onOpenSettingsClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onWorkingDirChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
					virtual void onSeriesChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onRunClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onAboutClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onAddChannelClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onAddCellApproxClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onAddIntersectionClicked( wxCommandEvent& event ) { event.Skip(); }


				public:
					wxChoice* mChoiceSeries;

					frameMain( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("imageC"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

					~frameMain();

			};

			///////////////////////////////////////////////////////////////////////////////
			/// Class DialogProcessing
			///////////////////////////////////////////////////////////////////////////////
			class DialogProcessing : public wxDialog
			{
				private:

				protected:
					wxStaticText* mLabelProgressImage;
					wxGauge* mProgressImage;
					wxStaticText* mLabelProgressAllOver;
					wxGauge* mProgressAllOver;
					wxStaticText* mLabelReporting;
					wxStaticLine* mLineProgressDialog;
					wxPanel* mPanelFooter;
					wxPanel* panelFooterButtons;
					wxStaticBitmap* m_bitmap21;
					wxStaticText* labelRAM;
					wxStaticBitmap* iconCpuCores;
					wxStaticText* labelAvailableCores;
					wxButton* mButtonStop;
					wxButton* mButtonClose;

					// Virtual event handlers, override them in your derived class
					virtual void onStopClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onCloseClicked( wxCommandEvent& event ) { event.Skip(); }


				public:

					DialogProcessing( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Analyze running..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 694,250 ), long style = wxDEFAULT_DIALOG_STYLE );

					~DialogProcessing();

			};

			///////////////////////////////////////////////////////////////////////////////
			/// Class DialogAbout
			///////////////////////////////////////////////////////////////////////////////
			class DialogAbout : public wxDialog
			{
				private:

				protected:
					wxStaticBitmap* mIconLogo;
					wxStaticText* mLabelTitle;

				public:

					DialogAbout( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Info"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 488,388 ), long style = wxDEFAULT_DIALOG_STYLE );

					~DialogAbout();

			};

			///////////////////////////////////////////////////////////////////////////////
			/// Class PanelChannel
			///////////////////////////////////////////////////////////////////////////////
			class PanelChannel : public wxPanel
			{
				private:

				protected:
					wxPanel* mPanelHeade;
					wxStaticText* mLabelChannelTitle;
					wxButton* mButtonRemoveChannel;
					wxPanel* mPanelToolbar;
					wxTextCtrl* mTextChannelName;
					wxChoice* mChoiceChannelType;
					wxChoice* mChoiceChannelIndex;
					wxBoxSizer* mSizerChannel;
					wxCollapsiblePane* mCollapaablePreprocessing;
					wxStaticLine* mLineDescription1;
					wxPanel* panelZStack;
					wxStaticBitmap* iconZStack;
					wxChoice* mChoiceZStack;
					wxStaticText* mLabelZStack;
					wxPanel* panelMarginCrop;
					wxStaticBitmap* iconMarginCrop;
					wxSpinCtrl* mSpinMarginCrop;
					wxStaticText* mLabelMarginCrop;
					wxPanel* panelMedianBGSubtract;
					wxStaticBitmap* iconMedianBGSubtract;
					wxChoice* mChoiceMedianBGSubtract;
					wxStaticText* mLabelMedianBGSubtract;
					wxPanel* panelEdgeDetection;
					wxStaticBitmap* iconEdgeDetection;
					wxChoice* mDropdownEdgeDetection;
					wxChoice* mDropdownEdgeDetectionDirection;
					wxStaticText* mLabelEdgeDetection;
					wxPanel* panelRollingBall;
					wxStaticBitmap* iconRollingBall;
					wxSpinCtrl* mSpinRollingBall;
					wxStaticText* mLabelRollingBall;
					wxPanel* panelBGSubtraction;
					wxStaticBitmap* iconBGSubtraction;
					wxChoice* mChoiceBGSubtraction;
					wxStaticText* mLabelBGSubtraction;
					wxPanel* panelSmoothing;
					wxStaticBitmap* iconSmoothing;
					wxChoice* mDropDownSmoothingRepeat;
					wxStaticText* mLabelSmoothing;
					wxPanel* panelGausianBlur;
					wxStaticBitmap* iconGausianBluer;
					wxChoice* mDropdownGausianBlur;
					wxChoice* mDropDownGausianBlurRepeat;
					wxStaticText* mLabelGausianBluer;
					wxPanel* m_panel31;
					wxCollapsiblePane* mCollapsibleDetection;
					wxStaticLine* mLineDescription;
					wxPanel* panelUseAI;
					wxStaticBitmap* iconUseAI;
					wxCheckBox* mCheckUseAI;
					wxPanel* panelThresholdMethod;
					wxStaticBitmap* iconThresholdMethod;
					wxChoice* mChoiceThresholdMethod;
					wxStaticText* mLabelThresholdMethod;
					wxPanel* panelMinThreshold;
					wxStaticBitmap* iconMinThreshold;
					wxSpinCtrl* mSpinMinThreshold;
					wxStaticText* mLabelMinThreshold;
					wxPanel* panelAImodel;
					wxStaticBitmap* iconAImodel;
					wxChoice* mChoiceAImodel;
					wxStaticText* mLabelAImodel;
					wxPanel* panelMinProbability;
					wxStaticBitmap* iconMinProbability;
					wxSpinCtrl* mSpinMinProbability;
					wxStaticText* mLabelMinProbability;
					wxCollapsiblePane* mCollapsibleFiltering;
					wxStaticLine* mLineFilter;
					wxPanel* panelMinCircularity;
					wxStaticBitmap* iconMinCircularity;
					wxSpinCtrlDouble* mSpinMinCircularity;
					wxStaticText* mLabelMinCircularity;
					wxPanel* panelParticleSize;
					wxStaticBitmap* iDescription1121;
					wxTextCtrl* mTextParticleSizeRange;
					wxStaticText* mLabelParticleSize;
					wxPanel* panelSnapArea;
					wxStaticBitmap* iconParticleSize;
					wxSpinCtrl* mSpinSnapArea;
					wxStaticText* mLabelSnapArea;
					wxPanel* panelReferenceChannel;
					wxStaticBitmap* iconTetraspeckChannel;
					wxChoice* mChoiceReferenceSpotChannel;
					wxStaticText* mLabelTetraspeckChannel;
					wxPanel* mPanelFooter;
					wxButton* mButtonPrev;
					wxButton* mPrevTile;
					wxButton* mButtonPreview1;
					wxButton* mNextTile;
					wxButton* mButtonNext;

					// Virtual event handlers, override them in your derived class
					virtual void onRemoveClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onChannelTypeChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onChannelIndexChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onCollapsibleChanged( wxCollapsiblePaneEvent& event ) { event.Skip(); }
					virtual void onZStackSettingsChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onMarginCropChanged( wxSpinEvent& event ) { event.Skip(); }
					virtual void onMedianBGSubtractChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onGausianBlurChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onGausianBlurRepeatChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onRollingBallChanged( wxSpinEvent& event ) { event.Skip(); }
					virtual void onBgSubtractChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onSmoothingChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onAiCheckBox( wxCommandEvent& event ) { event.Skip(); }
					virtual void onThresholdMethodChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onMinThresholdChanged( wxSpinEvent& event ) { event.Skip(); }
					virtual void onAImodelChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onAIMinProbabilityChanged( wxSpinEvent& event ) { event.Skip(); }
					virtual void onMinCircularityChanged( wxSpinDoubleEvent& event ) { event.Skip(); }
					virtual void onParticleSizeChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onSnapAreaChanged( wxSpinEvent& event ) { event.Skip(); }
					virtual void onSpotRemovalChanged( wxCommandEvent& event ) { event.Skip(); }
					virtual void onPrevImageClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onPrevTileClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onPreviewClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onNextTileClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onNextImageClicked( wxCommandEvent& event ) { event.Skip(); }


				public:
					wxBoxSizer* mSizerForScroll;
					wxScrolledWindow* mScrolledChannel;

					PanelChannel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

					~PanelChannel();

			};

			///////////////////////////////////////////////////////////////////////////////
			/// Class PanelIntersection
			///////////////////////////////////////////////////////////////////////////////
			class PanelIntersection : public wxPanel
			{
				private:

				protected:
					wxPanel* mPanelHeade;
					wxStaticText* mLabelChannelTitle;
					wxButton* mButtonRemoveChannel;
					wxScrolledWindow* mPanelIntersection;
					wxPanel* panelMinIntersection;
					wxStaticBitmap* iconMinIntersection;
					wxSpinCtrlDouble* mSpinMinIntersection;
					wxStaticText* mLabelMinIntersection;
					wxPanel* mPanelIntersectionButtons;
					wxToggleButton* mButtonIntersectionCh01;
					wxToggleButton* mButtonIntersectionCh02;
					wxToggleButton* mButtonIntersectionCh03;
					wxToggleButton* mButtonIntersectionCh04;
					wxToggleButton* mButtonIntersectionCh05;
					wxToggleButton* mButtonIntersectionCh06;
					wxToggleButton* mButtonIntersectionCh07;
					wxToggleButton* mButtonIntersectionCh08;
					wxToggleButton* mButtonIntersectionCh09;
					wxToggleButton* mButtonIntersectionCh10;
					wxToggleButton* mButtonIntersectionCh11;
					wxToggleButton* mButtonIntersectionCh12;
					wxToggleButton* mButtonIntersectionChEstimatedCell;

					// Virtual event handlers, override them in your derived class
					virtual void onRemoveClicked( wxCommandEvent& event ) { event.Skip(); }


				public:

					PanelIntersection( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

					~PanelIntersection();

			};

			///////////////////////////////////////////////////////////////////////////////
			/// Class PanelCellApproximation
			///////////////////////////////////////////////////////////////////////////////
			class PanelCellApproximation : public wxPanel
			{
				private:

				protected:
					wxBoxSizer* mSizerHorizontalScrolPipelineSteps;
					wxPanel* mPanelHeade;
					wxStaticText* mLabelChannelTitle;
					wxButton* mButtonRemoveChannel;
					wxScrolledWindow* mScrrollbarPipelineStep;
					wxScrolledWindow* panelPipelineStepCellApproximation;
					wxPanel* panelNucleusChannel;
					wxStaticBitmap* iconNucluesChannel;
					wxChoice* mChoiceNucluesChannel;
					wxStaticText* mLabelNucleusChannel;
					wxPanel* panelCellChannel;
					wxStaticBitmap* iconCellChannel;
					wxChoice* mChoiceCellChannel;
					wxStaticText* mLabelCellChannel;
					wxPanel* panelMaxCellRadius;
					wxStaticBitmap* iconMaxCellRadius;
					wxSpinCtrl* mSpinMaxCellRadius;
					wxStaticText* mLabelMaxCellRadius;

					// Virtual event handlers, override them in your derived class
					virtual void onRemoveClicked( wxCommandEvent& event ) { event.Skip(); }
					virtual void onCellChannelChoice( wxCommandEvent& event ) { event.Skip(); }


				public:

					PanelCellApproximation( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

					~PanelCellApproximation();

			};

			///////////////////////////////////////////////////////////////////////////////
			/// Class DialogImage
			///////////////////////////////////////////////////////////////////////////////
			class DialogImage : public wxDialog
			{
				private:

				protected:
					wxBoxSizer* mSizer;
					wxGauge* mImageDisplayProgress;
					wxStaticText* mImagePath;

				public:

					DialogImage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 538,345 ), long style = wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );

					~DialogImage();

			};

		} // namespace wxwidget
	} // namespace ui
} // namespace joda

