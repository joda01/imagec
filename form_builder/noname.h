///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-370-gc831f1f7)
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
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/statbmp.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/scrolwin.h>
#include <wx/tglbtn.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

namespace joda
{
	namespace gui
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
				wxTextCtrl* mTextWorkingDirectory;
				wxToolBarToolBase* mButtonSelectWorkingDirectory;
				wxToolBarToolBase* mButtonRun;
				wxToolBarToolBase* mButtonSettings;
				wxToolBarToolBase* mButtonAbout;
				wxNotebook* mNotebookMain;
				wxPanel* mPanelChannel;
				wxScrolledWindow* mPannelChannel;
				wxStaticText* mLabelChannelTitle;
				wxTextCtrl* mTextChannelName;
				wxPanel* panelChannelType;
				wxChoice* mChoiceChannelType;
				wxPanel* panelChannelIndex;
				wxChoice* mChoiceChannelIndex;
				wxStaticText* mLabelPreprocessing;
				wxStaticLine* mLinePreprocessing;
				wxPanel* panelZStack;
				wxStaticBitmap* iconZStack;
				wxChoice* mChoiceZStack;
				wxStaticText* mLabelZStack;
				wxPanel* panelMarginCrop;
				wxStaticBitmap* iconMarginCrop;
				wxSpinCtrlDouble* mSpinMarginCrop;
				wxStaticText* mLabelMarginCrop;
				wxPanel* panelMedianBGSubtract;
				wxStaticBitmap* iconMedianBGSubtract;
				wxChoice* mChoiceMedianBGSubtract;
				wxStaticText* mLabelMedianBGSubtract;
				wxPanel* panelRollingBall;
				wxStaticBitmap* iconRollingBall;
				wxSpinCtrlDouble* mSpinRollingBall;
				wxStaticText* mLabelRollingBall;
				wxPanel* panelBGSubtraction;
				wxStaticBitmap* iconBGSubtraction;
				wxChoice* mSpinBGSubtraction;
				wxStaticText* mLabelBGSubtraction;
				wxPanel* panelBluer;
				wxStaticBitmap* iconBluer;
				wxSpinCtrlDouble* mSpinBluer;
				wxStaticText* mLabelBluer;
				wxStaticText* mLabelDescription;
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
				wxSpinCtrlDouble* mSpinMinThreshold;
				wxStaticText* mLabelMinThreshold;
				wxStaticText* mLabelFilter;
				wxStaticLine* mLineFilter;
				wxPanel* panelMinCircularity;
				wxStaticBitmap* iconMinCircularity;
				wxSpinCtrlDouble* mSpinMinCircularity;
				wxStaticText* mLabelMinCircularity;
				wxPanel* panelParticleSize;
				wxStaticBitmap* iDescription1121;
				wxTextCtrl* m_textCtrl3;
				wxStaticText* lDescription1121;
				wxPanel* panelParticleSize111;
				wxStaticBitmap* iconParticleSize;
				wxSpinCtrlDouble* mSpinParticleSize;
				wxStaticText* mLabelParticleSize;
				wxStaticLine* mLinePreview;
				wxButton* mButtonPreview;
				wxStaticLine* mLineRemove;
				wxButton* mButtonRemoveChannel;
				wxButton* mButtonAddChannel;
				wxPanel* mPanelPipelineSteps;
				wxScrolledWindow* panelPipelineStepCellEstimation;
				wxStaticText* mLabelCellEstimation;
				wxPanel* panelNucleusChannel;
				wxStaticBitmap* iconNucluesChannel;
				wxChoice* mChoiceNucluesChannel;
				wxStaticText* mLabelNucleusChannel;
				wxPanel* panelMaxCellRadius;
				wxStaticBitmap* iconMaxCellRadius;
				wxSpinCtrlDouble* mSpinMaxCellRadius;
				wxStaticText* mLabelMaxCellRadius;
				wxScrolledWindow* mPanelSpotRemoval;
				wxStaticText* mLabelSpotRemoval;
				wxPanel* panelTetraspeckChannel;
				wxStaticBitmap* iconTetraspeckChannel;
				wxChoice* mChoiceTetraspeckChannel;
				wxStaticText* mLabelTetraspeckChannel;
				wxScrolledWindow* mPanelIntersection;
				wxStaticText* mLabelIntersection;
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
				wxStatusBar* m_statusBar1;

			public:

				frameMain( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("imageC"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1920,1080 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

				~frameMain();

		};

		///////////////////////////////////////////////////////////////////////////////
		/// Class dialogProcessing
		///////////////////////////////////////////////////////////////////////////////
		class dialogProcessing : public wxDialog
		{
			private:

			protected:
				wxStaticText* mLabelProgressImage;
				wxGauge* mProgressImage;
				wxStaticText* mLabelProgressAllOver;
				wxGauge* mProgressAllOver;
				wxStaticLine* mLineProgressDialog;
				wxPanel* mPanelFooter;
				wxPanel* panelFooterButtons;
				wxStaticBitmap* iconCpuCores;
				wxSpinCtrlDouble* mSpinCpuCores;
				wxButton* mButtonStart;
				wxButton* mButtonStop;
				wxStaticText* mLabelCpuCores;

			public:

				dialogProcessing( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Analyze running..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 694,191 ), long style = wxDEFAULT_DIALOG_STYLE );

				~dialogProcessing();

		};

		///////////////////////////////////////////////////////////////////////////////
		/// Class dialogAbout
		///////////////////////////////////////////////////////////////////////////////
		class dialogAbout : public wxDialog
		{
			private:

			protected:
				wxStaticBitmap* mIconLogo;
				wxStaticText* mLabelTitle;
				wxStaticText* mLabelDescription;

			public:

				dialogAbout( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Info"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 411,267 ), long style = wxDEFAULT_DIALOG_STYLE );

				~dialogAbout();

		};

	} // namespace gui
} // namespace joda

